#include "utils/string.h"
#include "realmode_int.h"


void vbe_switch_to_graphics() {
    regs16_t regs;
    regs.ax = 0x0013;
    r_int32(0x10, &regs);
}

void vbe_test_graphics() {
    _memset((char *)0xa0000, 0x9, (640*480));
}

void vbe_switch_to_text() {
    regs16_t regs;
    regs.ax = 0x0003;
    r_int32(0x10, &regs);
}

// int32 test
void int32_test() {
	/*
    int y;
    regs16_t regs;
    // switch to 320x200x256 graphics mode
    regs.ax = 0x0013;
    int32(0x10, &regs);
	
    // full screen with blue color (1)
    _memset((char *)0x000A0000, 1, (320*200));
	
    // draw horizontal line from 100,80 to 100,240 in multiple colors
    for(y = 0; y < 200; y++)
        _memset((char *)0x000A0000 + (y*320+80), y, 160);
    // wait for key
    regs.ax = 0x0000;
    int32(0x16, &regs);
	*/	

    //vbe_switch_to_graphics();
    vbe_test_graphics();

    //vbe_switch_to_text();
}
