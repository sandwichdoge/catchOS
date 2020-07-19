#include "drivers/cpuid.h"
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
#include "tasks.h"
#include "timer.h"
#include "utils/debug.h"
#include "utils/string.h"
#include "utils/list.h"

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

void test_multitask(void *done_cb) {
    for (int i = 0; i < 3; ++i) {
        _dbg_log("test\n");
        delay(100);
    }
    void (*fp)() = done_cb;
    fp();
}

int test_foreach(void* data) {
    _dbg_log("%d\n", *(int*)data);
    return 0;
}

void test_done_cb() {
    _dbg_log("Test done! Callback complete!\n");
    int n1 = 111, n2 = 000;
    struct list_head* testlist = list_create(&n1, sizeof(n1));
    testlist = list_insert_front(testlist, &n2, sizeof(n2));
    list_foreach(testlist, test_foreach);
    list_free(testlist);
}

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
    // test_memory_32bit_mode();
    task_new(test_multitask, test_done_cb, 1024, 5);

#ifdef WITH_GRUB_MB
    task_new(shell_main, mbinfo, 4096 * 4, 5);
#else
    task_new(shell_main, NULL, 4096 * 4, 5);
#endif
    asm("sti");  // Enable interrupts
    // task_yield();

    while (1) {
    }
}
