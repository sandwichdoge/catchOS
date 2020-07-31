#include "drivers/cpuid.h"
#include "drivers/framebuffer.h"
#include "drivers/serial.h"
#include "drivers/svga.h"
#include "drivers/acpi/acpi.h"
#include "interrupt.h"
#include "kheap.h"
#include "kinfo.h"
#include "mmu.h"
#include "multiboot.h"
#include "sem.h"
#include "shell.h"
#include "stddef.h"
#include "stdint.h"
#include "syscall.h"
#include "tasks.h"
#include "timer.h"
#include "utils/debug.h"
#include "utils/list.h"
#include "utils/string.h"

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

void test_done_cb() { _dbg_log("Test done! Callback complete!\n"); }

void kmain(unsigned int magic, unsigned int addr) {
    // First thing first, gather all info about our hardware capabilities, store it in kinfo singleton
    serial_defconfig(SERIAL_COM1_BASE);
    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        _dbg_log("Invalid mb magic:[0x%x]\n", magic);
        _dbg_break();
    }
    if (addr & 7) {
        _dbg_log("Unaligned mbi: 0x%x\n", addr);
        _dbg_break();
    }

#ifdef WITH_GRUB_MB
    kinfo_init((struct multiboot_tag *)addr);
#else
    kinfo_init(NULL);
#endif
    _dbg_log("kmain\n");

    // Setup interrupts
    interrupt_init();
    timer_init();

    kheap_init();
    mmu_init();
    syscall_init();
    svga_init();
    acpi_init();
    //char* s5 = acpi_dsdt_get_s5obj();
    //_dbg_screen("s5 object: 0x%x\n", s5);

    // Perform tests
    // test_memory_32bit_mode();
/*
    sem_init(&s, 1);
    struct task_struct *t1 = task_new(test_multitask, (void *)test_done_cb, 1024 * 2, 10);
    struct task_struct *t2 = task_new(test_multitask, (void *)test_done_cb, 1024 * 2, 10);
    struct task_struct *t3 = task_new(test_multitask, (void *)test_done_cb, 1024 * 2, 10);
    task_detach(t1);
    task_detach(t2);
    task_detach(t3);
*/
    task_new(shell_main, NULL, 4096 * 16, 10);
    asm("sti");  // Enable interrupts
    // task_yield();

    while (1) {
    }
}
