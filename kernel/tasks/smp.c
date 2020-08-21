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

struct _smpboot_trampoline_params {
    size_t stackbase;
    size_t idt;
    size_t kernel_pd;
};

public
void smp_init() {
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

    lapic_init(local_apic_base);
    _dbg_log("Enabling APIC - base [0x%x]\n", local_apic_base);
    lapic_enable(local_apic_base);
    lapic_send_init(local_apic_base, 1);
    delay_bootstrap(10);
    lapic_send_startup(local_apic_base, 1, (size_t)&SMPBOOT_TRAMPOLINE_FUNC);
    delay_bootstrap(10);
    lapic_send_startup(local_apic_base, 1, (size_t)&SMPBOOT_TRAMPOLINE_FUNC);
}
