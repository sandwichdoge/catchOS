#include "builddef.h"
#include "drivers/acpi/fadt.h"
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

void shutdown() {
    char *s5obj = NULL;
    s5obj = acpi_dsdt_get_s5obj();
    _dbg_screen("Found s5obj 0x%x\n", s5obj);
    // Check for valid AML structure
    if ((*(s5obj - 1) == 0x08 || (*(s5obj - 2) == 0x08 && *(s5obj - 1) == '\\')) && *(s5obj + 4) == 0x12) {
        s5obj += 5;
        s5obj += ((*s5obj & 0xC0) >> 6) + 2;  // Calculate PkgLength size

        if (*s5obj == 0x0A) s5obj++;  // Skip byteprefix
        unsigned char SLP_TYPa = *(s5obj) << 10;
        s5obj++;

        unsigned short SLP_EN = 1 << 13;
        uint32_t PM1a_CNT = acpi_get_fadt()->PM1aControlBlock;
        outw(PM1a_CNT, SLP_TYPa | SLP_EN);
    } else {
        _dbg_log("Parse error\n");
    }
}
