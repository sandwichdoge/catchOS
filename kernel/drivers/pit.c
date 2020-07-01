#include "pit.h"
#include "io.h"
#include "builddef.h"

#define PIT_DATA_CH0_PORT 0x40
#define PIT_DATA_CH1_PORT 0x41
#define PIT_DATA_CH2_PORT 0x42
#define PIT_COMMAND_PORT 0x43

public void pit_defconfig() {
    unsigned char conf = 0;
    // 00111100b
    conf = 0 << 7 | 0 << 6 |
           1 << 5 | 1 << 4 |
           1 << 3 | 1 << 2 | 0 << 1 |
           0;
    //outb(PIT_COMMAND_PORT, conf);
}