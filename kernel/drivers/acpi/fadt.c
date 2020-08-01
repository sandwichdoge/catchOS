#include "drivers/acpi/fadt.h"
#include "drivers/io.h"
#include "kinfo.h"
#include "pageframe_alloc.h"
#include "paging.h"
#include "utils/debug.h"
#include "utils/string.h"

static struct FADT* _fadt = NULL;
struct ACPISDTHeader* _dsdt = NULL;

struct FADT* acpi_get_fadt() {
    int acpi_ver = get_kernel_info()->acpi_ver;
    // TODO ask RSDT for saved addr of fadt instead
    return NULL;
}

struct ACPISDTHeader* acpi_get_dsdt() {
    int acpi_ver = get_kernel_info()->acpi_ver;
    if (!_dsdt) {
        struct FADT* fadt = acpi_get_fadt();
        if (acpi_ver == 1) {
            _dsdt = (struct ACPISDTHeader*)(fadt->Dsdt);
        } else {
            _dsdt = (struct ACPISDTHeader*)(fadt->X_Dsdt);
        }
        // Need to map again because DSDT is a sub-table of FADT, not an RSDT entry, it wasn't mapped during ACPI init.
        int is_paging_enabled = get_kernel_info()->is_paging_enabled;
        if (is_paging_enabled) {
            pageframe_set_page_from_addr((void*)_dsdt, 2);
            paging_map_page(_dsdt, _dsdt, get_kernel_pd());
            paging_map_page((char*)_dsdt + PAGE_SIZE, (char*)_dsdt + PAGE_SIZE, get_kernel_pd());

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

// S5 Object is basically an encoded device tree. It contains info on the hardware that's hooked into our system.
char* acpi_dsdt_get_s5obj() {
    struct ACPISDTHeader* h = acpi_get_dsdt();  // DSDT has a header just like other tables.
    _dbg_screen("DSDT: 0x%x\n", h);
    if (!sdt_checksum_ok(h) || _strncmp(h->Signature, "DSDT", 4) != 0) {
        _dbg_screen("Invalid DSDT\n");
        return NULL;
    } else {
        _dbg_screen("DSDT len %u\n", h->Length);
        char* dsdt_noheader = (char*)h + sizeof(*h);  // Skip SDT header
        char* s5obj = _strnstr(dsdt_noheader, "_S5_", h->Length);
        _dbg_break();
        return s5obj;
    }
}
