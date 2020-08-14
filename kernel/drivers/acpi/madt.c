#include "drivers/acpi/madt.h"

static struct MADT* _madt = NULL;

struct MADT* acpi_get_madt() {
    if (!_madt) {
        _madt = acpi_get_sdt_from_sig("APIC");
        _dbg_log("Found madt at 0x%x\n", _madt);
        _dbg_screen("Found madt at 0x%x\n", _madt);
    }
    return _madt;
}

