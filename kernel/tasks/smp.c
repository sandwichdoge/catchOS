#include "stddef.h"
#include "stdint.h"
#include "drivers/acpi/madt.h"
#include "drivers/lapic.h"
#include "pageframe_alloc.h"
#include "paging.h"
#include "utils/debug.h"
#include "builddef.h"
#include "timer.h"

//http://www.osdever.net/tutorials/view/multiprocessing-support-for-hobby-oses-explained

void trampoline() {
    // Setup kernel stack, GDT, IDT, Paging
    asm("cli;\n");
    asm("movl %esp, 0x2fffff;hlt;");
    _dbg_log("CPU1\n");
} __attribute__((aligned(4096)))

public
void smp_init() {
    struct MADT_info *madt_info = madt_get_info();
    size_t local_apic_base = (size_t)madt_info->local_apic_addr;
    paging_map_page(local_apic_base, local_apic_base, get_kernel_pd());
    pageframe_set_page_from_addr((void*)local_apic_base, 1);

    _dbg_log("Enabling APIC - base [0x%x]\n", local_apic_base);
    lapic_enable(local_apic_base);
    lapic_send_init(local_apic_base, 1);
    delay(10);
    lapic_send_startup(local_apic_base, 1, (size_t)&trampoline);
    delay(10);
    lapic_send_startup(local_apic_base, 1, (size_t)&trampoline);
}
