#include "stddef.h"
#include "stdint.h"
#include "drivers/acpi/madt.h"
#include "drivers/apic.h"
#include "pageframe_alloc.h"
#include "paging.h"
#include "utils/debug.h"
#include "builddef.h"

//http://www.osdever.net/tutorials/view/multiprocessing-support-for-hobby-oses-explained

public
void smp_init() {
    struct MADT_info *madt_info = madt_get_info();
    size_t local_apic_base = (size_t)madt_info->local_apic_addr;
    paging_map_page(local_apic_base, local_apic_base, get_kernel_pd());
    pageframe_set_page_from_addr(local_apic_base, 1);

    _dbg_log("Enabling APIC - base [0x%x]\n", local_apic_base);
    apic_enable(local_apic_base);
}

