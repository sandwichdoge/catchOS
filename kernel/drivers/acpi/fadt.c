#include "drivers/acpi/acpi.h"
#include "drivers/acpi/fadt.h"
#include "utils/string.h"

struct FADT* acpi_get_fadt() {
    struct RSDT *rsdt = acpi_get_rsdt();
    if (!rsdt) return NULL;

    int entries = (rsdt->h.Length - sizeof(struct ACPISDTHeader)) / 4;
    for (int i = 0; i < entries; ++i) {
        uint32_t* tail = &rsdt->others;
        tail += i;
        struct ACPISDTHeader *sdt = *tail;
        if (_strncmp(sdt->Signature, "FACP", 4) == 0) {
            return (struct FADT*)sdt;
        }
    }
    _dbg_log("FADT not found\n");
    return NULL;
}
