#include "builddef.h"
#include "drivers/acpi/fadt.h"
#include "drivers/io.h"
#include "stddef.h"
#include "stdint.h"
#include "utils/debug.h"

//
// bytecode of the \_S5 object
// -----------------------------------------
//        | (optional) |    |    |    |
// NameOP | \          | _  | S  | 5  | _
// 08     | 5A         | 5F | 53 | 35 | 5F
//
// -----------------------------------------------------------------------------------------------------------
//           |           |              | ( SLP_TYPa   ) | ( SLP_TYPb   ) | ( Reserved   ) | (Reserved    )
// PackageOP | PkgLength | NumElements  | byteprefix Num | byteprefix Num | byteprefix Num | byteprefix Num
// 12        | 0A        | 04           | 0A         05  | 0A          05 | 0A         05  | 0A         05
//
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

void shutdown() {
    char* s5obj = NULL;
    s5obj = acpi_dsdt_get_s5obj();
    _dbg_screen("Found s5obj 0x%x\n", s5obj);
    // Check for valid AML structure
    if ((*(s5obj - 1) == 0x08 || (*(s5obj - 2) == 0x08 && *(s5obj - 1) == '\\')) && *(s5obj + 4) == 0x12) {
        s5obj += 5;
        s5obj += ((*s5obj & 0xC0) >> 6) + 2;  // Calculate PkgLength size

        if (*s5obj == 0x0A) s5obj++;  // Skip byteprefix
        uint8_t SLP_TYPa = *(s5obj) << 10;
        s5obj++;

        if (*s5obj == 0x0A) s5obj++;  // Skip byteprefix
        uint8_t SLP_TYPb = *(s5obj) << 10;

        unsigned short SLP_EN = 1 << 13;
        uint32_t PM1a_CNT = acpi_get_fadt()->PM1aControlBlock;
        uint32_t PM1b_CNT = acpi_get_fadt()->PM1bControlBlock;
        _dbg_screen("Shutting down. PM1a_CNT[%u], SLP_TYPa[%u], PM1b_CNT[%u], SLP_TYPb[%u]\n", PM1a_CNT, SLP_TYPa, PM1b_CNT, SLP_TYPb);
        outw(PM1a_CNT, SLP_TYPa | SLP_EN);
        if (SLP_TYPb) outw(PM1b_CNT, SLP_TYPb | SLP_EN);
        _dbg_screen("Power off failed.\n");
    } else {
        _dbg_log("Parse error\n");
    }
}
