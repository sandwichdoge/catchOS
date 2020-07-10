#include "drivers/framebuffer.h"
#include "drivers/serial.h"
#include "interrupt.h"
#include "kheap.h"
#include "kinfo.h"
#include "mmu.h"
#include "multiboot.h"
#include "shell.h"
#include "stddef.h"
#include "stdint.h"
#include "syscall.h"
#include "timer.h"
#include "tasks.h"
#include "utils/debug.h"
#include "utils/string.h"

void test_memory_32bit_mode() {
    volatile unsigned char *p = (volatile unsigned char *)(0xc0000000 + 3000000);  // 32MB for testing 32-bit mode
    *p = 55;

    if (*p == 55) {
        write_cstr("Memtest success.", 80);
    } else {
        write_cstr("Memtest failed.", 80);
        _dbg_break();
    }
}

void halt() { asm("hlt"); }

void kmain(unsigned int ebx) {
// First thing first, gather all info about our hardware capabilities, store it in kinfo singleton
#ifdef WITH_GRUB_MB
    multiboot_info_t *mbinfo = (multiboot_info_t *)ebx;
    kinfo_init((multiboot_info_t *)ebx);
#else
    kinfo_init(NULL);
#endif

    serial_defconfig(SERIAL_COM1_BASE);
    
    // Setup interrupts
    write_cstr("Setting up interrupts..", 0);
    interrupt_init();
    timer_init();

    write_cstr("Setting up memory..", 80);
    kheap_init();
    mmu_init();
    syscall_init();

    // Perform memory tests
    test_memory_32bit_mode();

    test_caller();

    asm("sti");  // Enable interrupts
    // TODO Bug, hang on serial_write() after sti is called

    // Enter I/O shell
    #ifdef WITH_GRUB_MB
        shell_main(mbinfo);
    #else
        shell_main(NULL);
    #endif

    while (1) {
    }
}
