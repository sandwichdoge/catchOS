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
#include "sem.h"
#include "utils/debug.h"
#include "utils/string.h"
#include "utils/list.h"
#include "shell_graphical.h"

void test_memory_32bit_mode() {
    volatile unsigned char *p = (volatile unsigned char *)(0x0 + 3000000);  // 32MB for testing 32-bit mode
    *p = 55;

    if (*p == 55) {
        write_cstr("Memtest success.", 80);
    } else {
        write_cstr("Memtest failed.", 80);
        _dbg_break();
    }
}

void halt() { asm("hlt"); }

struct semaphore s;
void test_multitask(void *done_cb) {
    _dbg_log("test start\n");
    sem_wait(&s);
    for (int i = 0; i < 4; ++i) {
        _dbg_log("[pid %u]test\n", task_getpid());
        delay(100);
    }
    void (*fp)() = done_cb;
    fp();
    sem_signal(&s);
}

void test_done_cb() {
    _dbg_log("Test done! Callback complete!\n");
}

extern void int32_test();

void kmain(unsigned int ebx) {
// First thing first, gather all info about our hardware capabilities, store it in kinfo singleton
#ifdef WITH_GRUB_MB
    multiboot_info_t *mbinfo = (multiboot_info_t *)ebx;
    kinfo_init((multiboot_info_t *)ebx);
#else
    kinfo_init(NULL);
#endif
    serial_defconfig(SERIAL_COM1_BASE);
    _dbg_log("kmain\n");

    // Setup interrupts
    write_cstr("Setting up interrupts..", 0);
    interrupt_init();
    timer_init();

    write_cstr("Setting up memory..", 80);
    kheap_init();
    mmu_init();
    syscall_init();

    // Perform tests
    // test_memory_32bit_mode();
    
    sem_init(&s, 1);
    struct task_struct *t1 = task_new(test_multitask, (void*)test_done_cb, 1024 * 2, 10);
    struct task_struct *t2 = task_new(test_multitask, (void*)test_done_cb, 1024 * 2, 10);
    struct task_struct *t3 = task_new(test_multitask, (void*)test_done_cb, 1024 * 2, 10);
    task_detach(t1);
    task_detach(t2);
    task_detach(t3);

    int32_test();


#ifdef WITH_GRUB_MB
    task_new(shell_main, mbinfo, 4096 * 4, 10);
#else
    task_new(shell_main, NULL, 4096 * 4, 10);
#endif
    asm("sti");  // Enable interrupts
    // task_yield();

    while (1) {
    }
}
