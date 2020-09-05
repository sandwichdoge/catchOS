#include "builddef.h"
#include "drivers/acpi/madt.h"
#include "drivers/ioapic.h"
#include "drivers/lapic.h"
#include "drivers/pic.h"
#include "interrupt.h"  // interrupt_get_idt(), IRQ_REDIR_BASE
#include "kheap.h"
#include "pageframe_alloc.h"
#include "paging.h"
#include "stddef.h"
#include "stdint.h"
#include "timer.h"
#include "utils/debug.h"

// http://www.osdever.net/tutorials/view/multiprocessing-support-for-hobby-oses-explained
// We assume all local APICs and their CPUs have the same ID.

extern size_t SMPBOOT_TRAMPOLINE_FUNC;
extern size_t SMPBOOT_TRAMPOLINE_PARAMS;

uint8_t AP_STARTUP_SUCCESSFUL = 0;  // Trampoline code will set this.

struct _smpboot_trampoline_params {
    size_t stackbase;
    size_t idt;
    size_t kernel_pd;
    size_t lapic_base;
};

// Init,prepare kernel stack for new AP.
private
void prepare_trampoline_params() {
    // Setup params
    struct _smpboot_trampoline_params smp_params;
    smp_params.stackbase = (size_t)kmalloc(4096) + 4096;
    smp_params.idt = (size_t)interrupt_get_idt();    // 1 idt singleton
    smp_params.kernel_pd = (size_t)get_kernel_pd();  // 1 pd singleton
    struct MADT_info* madt_info = madt_get_info();
    size_t local_apic_base = (size_t)madt_info->local_apic_addr;
    smp_params.lapic_base = local_apic_base;

    // SMPBOOT_TRAMPOLINE_PARAMS is located in trampoline.S, it's used to pass arguments when booting a new AP.
    _memcpy(&SMPBOOT_TRAMPOLINE_PARAMS, &smp_params, sizeof(smp_params));
}

private
int32_t start_AP(size_t local_apic_base, uint8_t lapic_id) {
    // https://wiki.osdev.org/Symmetric_Multiprocessing#Startup_Sequence
    _dbg_screen("Starting LAPIC %d\n", lapic_id);

    prepare_trampoline_params();

    lapic_send_init(local_apic_base, lapic_id);
    delay_bootstrap(10);

    lapic_send_startup(local_apic_base, lapic_id, (size_t)&SMPBOOT_TRAMPOLINE_FUNC);
    delay_bootstrap(1);
    if (AP_STARTUP_SUCCESSFUL) {
        goto success;
    }

    lapic_send_startup(local_apic_base, lapic_id, (size_t)&SMPBOOT_TRAMPOLINE_FUNC);
    delay_bootstrap(1000);
    if (AP_STARTUP_SUCCESSFUL) {
        goto success;
    } else {  // Second try failed, stop trying.
        goto fail;
    }

success:
    AP_STARTUP_SUCCESSFUL = 0;  // Reset this flag for reuse on next CPU startup
    _dbg_log("AP[%u] startup successful.\n", lapic_id);
    _dbg_screen("AP[%u] startup successful.\n", lapic_id);
    return 0;

fail:
    _dbg_log("Failed to start up AP [%u].\n", lapic_id);
    _dbg_screen("Failed to start up AP [%u].\n", lapic_id);
    return -1;
}

private
int32_t start_APs() {
    struct MADT_info* madt_info = madt_get_info();
    size_t local_apic_base = (size_t)madt_info->local_apic_addr;
    paging_map_page(local_apic_base, local_apic_base, get_kernel_pd());
    pageframe_set_page_from_addr((void*)local_apic_base, 1);

    lapic_init(local_apic_base);  // Enable LAPIC and handle APIC spurious irqs.

    // Begin SMP startup sequence
    asm("sti");  // Need PIT to boot smp
    timer_init_bootstrap(1000);

    // Do not start BSP (first CPU), otherwise triple fault.
    for (uint8_t i = 0; i < madt_info->processor_count; ++i) {
        uint8_t lapic_id = madt_info->local_APIC_ids[i];
        if (lapic_id != 0) {  // Do not try to start BSP (LAPIC #0).
            start_AP(local_apic_base, lapic_id);
        }
    }
    asm("cli");

    return 0;
}

private
int32_t init_io_apic() {
    struct MADT_info* madt_info = madt_get_info();
    if (madt_info->io_apic_count == 0) {  // No APICs available (Intel 8086).
        return -1;
    }
    pic_uninit();
    for (uint16_t i = 0; i < madt_info->io_apic_count; ++i) {
        paging_map_page(madt_info->io_apic_addrs[i], madt_info->io_apic_addrs[i], get_kernel_pd());
        pageframe_set_page_from_addr((void*)(madt_info->io_apic_addrs[i]), 1);
        ioapic_init(madt_info->io_apic_addrs[i]);
    }
    return 0;
}

public
uint8_t smp_get_cpu_count() {
    return madt_get_info()->processor_count;
}

public
uint8_t smp_get_cpu_id() {
    struct MADT_info* madt_info = madt_get_info();
    return lapic_get_id((size_t)madt_info->local_apic_addr);
}

/* For now we only use physical destination mode, that means one irq may only be sent to 1 local APIC.
 * If we want to send an irq to a bunch of local APICs then we'll have to use logical destination mode.
 */
public
void smp_redirect_external_irq(uint8_t irq, uint8_t dest_cpu) {
    uint8_t phys_irq = irq - IRQ_REDIR_BASE;
    if (ioapic_is_initialized()) {
        ioapic_redirect_external_int(phys_irq, dest_cpu);
    }
}

public
void smp_init() {
    start_APs();
    init_io_apic();
}
