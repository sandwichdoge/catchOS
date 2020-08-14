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
            acpi_map_sdt(_dsdt);
        }
    }
    _dbg_log("DSDT at 0x%x\n", _dsdt);
    return _dsdt;
}
