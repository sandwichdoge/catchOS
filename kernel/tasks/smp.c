#include "stddef.h"
#include "stdint.h"
#include "drivers/acpi/madt.h"
#include "drivers/lapic.h"
#include "pageframe_alloc.h"
#include "paging.h"
#include "utils/debug.h"
#include "builddef.h"
#include "timer.h"
#include "interrupt.h"  // interrupt_get_idt()
#include "kheap.h"

//http://www.osdever.net/tutorials/view/multiprocessing-support-for-hobby-oses-explained

extern size_t SMPBOOT_TRAMPOLINE_FUNC;
extern size_t SMPBOOT_TRAMPOLINE_PARAMS;

uint8_t AP_STARTUP_SUCCESSFUL = 0;   // Trampoline code will set this.

struct _smpboot_trampoline_params {
    size_t stackbase;
    size_t idt;
    size_t kernel_pd;
};

private
int32_t start_APs() {
    struct MADT_info *madt_info = madt_get_info();
    size_t local_apic_base = (size_t)madt_info->local_apic_addr;
    paging_map_page(local_apic_base, local_apic_base, get_kernel_pd());
    pageframe_set_page_from_addr((void*)local_apic_base, 1);

    // Setup params
    size_t stackbase = (size_t)kmalloc(4096) + 4096;
    size_t idt = (size_t)interrupt_get_idt();   // 1 idt singleton
    size_t kernel_pd = (size_t)get_kernel_pd(); // 1 pd singleton
    _dbg_log("Allocated kstack for CPU %d at [0x%x]\n", 1, stackbase);

    struct _smpboot_trampoline_params smp_params;
    smp_params.stackbase = stackbase;
    smp_params.idt = idt;
    smp_params.kernel_pd = kernel_pd;
    _memcpy(&SMPBOOT_TRAMPOLINE_PARAMS, &smp_params, sizeof(smp_params));

    // Begin SMP startup sequence
    asm("sti");  // Need PIT to boot smp
    timer_init_bootstrap(1000);

    lapic_init(local_apic_base);
    lapic_enable(local_apic_base);  // Enable BSP's LAPIC just in case.

    // https://wiki.osdev.org/Symmetric_Multiprocessing#Startup_Sequence
    lapic_send_init(local_apic_base, 1);
    delay_bootstrap(10);

    lapic_send_startup(local_apic_base, 1, (size_t)&SMPBOOT_TRAMPOLINE_FUNC);
    delay_bootstrap(1);
    if (AP_STARTUP_SUCCESSFUL)
        goto success;

    lapic_send_startup(local_apic_base, 1, (size_t)&SMPBOOT_TRAMPOLINE_FUNC);
    delay_bootstrap(1000);
    if (!AP_STARTUP_SUCCESSFUL)
        goto fail;

fail:
    _dbg_log("Failed to start up AP.\n");
    asm("cli");
    return -1;
success:
    AP_STARTUP_SUCCESSFUL = 0;  // Reset this flag for reuse on next CPU startup
    _dbg_log("AP startup successful.\n");
    asm("cli");
    return 0;
}

private
int32_t init_io_apic() {
    return 0;
}

public
void smp_init() {
    start_APs();
    init_io_apic();
}
