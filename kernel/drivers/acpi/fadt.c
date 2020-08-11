#include "drivers/acpi/fadt.h"
#include "drivers/io.h"
#include "kinfo.h"
#include "pageframe_alloc.h"
#include "paging.h"
#include "utils/debug.h"

static struct ACPISDTHeader* _dsdt = NULL;
static struct FADT* _fadt = NULL;

struct FADT* acpi_get_fadt() {
    if (!_fadt) {
        _fadt = acpi_get_sdt_from_sig("FACP");
        // Enable ACPI
        outb(_fadt->SMI_CommandPort, _fadt->AcpiEnable);
        _dbg_log("Found FADT at 0x%x\n", _fadt);
        _dbg_screen("Found FADT at 0x%x\n", _fadt);
    }
    return _fadt;
}

struct ACPISDTHeader* acpi_get_dsdt() {
    if (!_dsdt) {
        struct FADT* fadt = acpi_get_fadt();
        _dsdt = (struct ACPISDTHeader*)(fadt->Dsdt);
        _dbg_screen("DSDT at 0x%x\n", _dsdt);
        // Need to map again because DSDT is a sub-table of FADT, not an RSDT entry, it wasn't mapped during ACPI init.
        int is_paging_enabled = get_kernel_info()->is_paging_enabled;
        if (is_paging_enabled) {
            pageframe_set_page_from_addr((void*)_dsdt, 2);
            paging_map_page((unsigned int)_dsdt, (unsigned int)_dsdt, get_kernel_pd());
            paging_map_page((unsigned int)_dsdt + PAGE_SIZE, (unsigned int)_dsdt + PAGE_SIZE, get_kernel_pd());

            uint32_t start_page = (uint32_t)_dsdt / PAGE_SIZE;
            uint32_t end_page = ((uint32_t)_dsdt + _dsdt->Length) / PAGE_SIZE;
            uint32_t pages_to_alloc = end_page - start_page + 1;
            _dbg_screen("sdt[0x%x], start[%u], len[0x%x], pages_to_alloc:%u, sig[0x%x]\n", _dsdt, start_page, _dsdt->Length, pages_to_alloc, _dsdt->Signature);
            pageframe_set_page_from_addr((void*)_dsdt, pages_to_alloc);
            for (uint32_t j = 0; j < pages_to_alloc; ++j) {
                paging_map_page((uint32_t)_dsdt + j * PAGE_SIZE, (uint32_t)_dsdt + j * PAGE_SIZE, get_kernel_pd());
            }
        }
    }
    _dbg_log("DSDT at 0x%x\n", _dsdt);
    return _dsdt;
}
