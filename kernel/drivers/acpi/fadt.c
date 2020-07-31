#include "drivers/acpi/acpi.h"
#include "drivers/acpi/fadt.h"
#include "utils/string.h"
#include "utils/debug.h"

static struct FADT* _fadt = NULL;

struct FADT* acpi_get_fadt() {
    if (_fadt) {
        return _fadt;
    } else {
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
}

char* acpi_get_dsdt() {
    struct FADT* fadt = acpi_get_fadt();
    return (char*)fadt->Dsdt;
}

char* acpi_dsdt_get_s5obj() {
    struct ACPISDTHeader *h = (struct ACPISDTHeader *)acpi_get_dsdt();
    if (_strncmp(h->Signature, "DSDT", 4) != 0) {
        _dbg_log("Invalid DSDT\n");
        return NULL;
    } else {
        return (char*)h + sizeof(*h); // Skip SDT header
    }
}
