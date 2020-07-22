#include "drivers/svga.h"

#include "kinfo.h"
#include "realmode_int.h"
#include "stdint.h"
#include "timer.h"
#include "utils/debug.h"

typedef struct svga_control_info {
    char VbeSignature[4];      // == "VESA"
    uint16_t VbeVersion;       // == 0x0300 for VBE 3.0
    uint16_t OemStringPtr[2];  // isa vbeFarPtr
    uint8_t Capabilities[4];
    uint16_t VideoModePtr[2];  // isa vbeFarPtr
    uint16_t TotalMemory;      // as # of 64KB blocks
} __attribute__((packed)) svga_control_info_t;

typedef struct svga_mode_info {
    uint16_t attributes;
    uint8_t windowA, windowB;
    uint16_t granularity;
    uint16_t windowSize;
    uint16_t segmentA, segmentB;
    uint32_t winFuncPtr;  // ptr to INT 0x10 Function 0x4F05
    uint16_t pitch;       // bytes per scan line

    uint16_t screen_width, screen_height;      // resolution
    uint8_t wChar, yChar, planes, bpp, banks;  // number of banks
    uint8_t memoryModel, bankSize, imagePages;
    uint8_t reserved0;

    // color masks
    uint8_t readMask, redPosition;
    uint8_t greenMask, greenPosition;
    uint8_t blueMask, bluePosition;
    uint8_t reservedMask, reservedPosition;
    uint8_t directColorAttributes;

    uint32_t physbase;  // pointer to LFB in LFB modes
    uint32_t offScreenMemOff;
    uint16_t offScreenMemSize;
} __attribute__((packed)) svga_mode_info_t;
/*
svga_mode_info_t* svga_get_vbe_modeinfo() {
    struct kinfo* kinfo = get_kernel_info();
    multiboot_info_t* mbinfo = kinfo->mbinfo;
    svga_mode_info_t* vbe_info = (svga_mode_info_t*)mbinfo->vbe_mode_info;
    return vbe_info;
}

svga_control_info_t* svga_get_vbe_controlinfo() {
    struct kinfo* kinfo = get_kernel_info();
    multiboot_info_t* mbinfo = kinfo->mbinfo;
    _dbg_log("EXPECT [%u] == 1\n", mbinfo->mods_count);
    svga_control_info_t* control_info = (svga_control_info_t*)mbinfo->vbe_control_info;
    return control_info;
}

void get_physbase() {
    svga_control_info_t* control_info = svga_get_vbe_controlinfo();
    _dbg_log("control:0x%x\n", control_info);
    svga_mode_info_t* vbe_info = svga_get_vbe_modeinfo();
    _dbg_log("mode:0x%x\n", vbe_info);

    _dbg_log("svga physbase: 0x%x\n", vbe_info->physbase);
    _dbg_log("scr w/h [%u/%u]\n", vbe_info->screen_width, vbe_info->screen_height);
    _dbg_log("svga planes: [%u]\n", vbe_info->planes);
    _dbg_log("svga winsize [%u]\n", vbe_info->windowSize);

    regs16_t regs;
    regs.ax = 0x4f02;
    regs.bx = 0x111 | 0x4000;
    r_int32(0x10, &regs);
    _memset((char*)vbe_info->physbase, 0x9, (640 / 8) * 480);
}
*/