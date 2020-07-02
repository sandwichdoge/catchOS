#include "interrupt.h"
#include "builddef.h"
#include "drivers/keyboard.h"
#include "drivers/pic.h"
#include "syscall.h"  // _kb_handler_cb()
#include "utils/debug.h"

#define IDT_SIZE 256

extern void asm_int_handler_32();   // Handler for PIT channel0 system timer
extern void asm_int_handler_33();   // Handler for keyboard press
extern void asm_int_handler_14();   // Handler for page fault
extern void asm_int_handler_128();  // Handler for syscall

struct idt IDT;                                // To be loaded into the CPU
struct idt_entry idt_entries[IDT_SIZE] = {{0}};  // Main content of IDT
// Array of void func(uint*, cpu_state*) pointers
void (*int_handler_table[IDT_SIZE])(unsigned int* return_reg, struct cpu_state*) = {0}; 

// https://wiki.osdev.org/Interrupt_Descriptor_Table
private
void interrupt_encode_idt_entry(unsigned int interrupt_num, unsigned int f_ptr_handler) {
    idt_entries[interrupt_num].offset_low = f_ptr_handler & 0xffff;
    idt_entries[interrupt_num].offset_high = (f_ptr_handler >> 16) & 0xffff;

    idt_entries[interrupt_num].segment_selector = 0x8;  // code segment in gdt
    idt_entries[interrupt_num].reserved = 0x0;

    idt_entries[interrupt_num].type_and_attr = (1 << 7) |             // P
                                               (0 << 6) | (0 << 5) |  // DPL
                                               (0 << 4) |             // S
                                               0xe;                   // 32-bit interrupt gate - 0b1110
}

private
void lidt(struct idt* idt_r) { asm("lidt %0" ::"m"(*idt_r)); }

private
void ISR_KEYBOARD(unsigned int* return_reg, struct cpu_state* unused) {
    static int is_shift_key_depressed = 0;
    unsigned char scan_code = read_scan_code();
    unsigned char ascii = scan_code_to_ascii(scan_code, is_shift_key_depressed);
    if (ascii == KEY_LSHIFT || ascii == KEY_RSHIFT) {
        is_shift_key_depressed ^= 1;
    } else if (_kb_handler_cb) {
        _kb_handler_cb(ascii);
    }
}

private
void ISR_PAGEFAULT(unsigned int* return_reg, struct cpu_state* unused) {
    _dbg_log("Error. Pagefault!\n");
    _dbg_break();
}

// int 128 (or 0x80). Syscalls may modify eax, ecx, e11.
private
void ISR_SYSCALL(unsigned int* return_reg, struct cpu_state* regs) { 
    syscall_handler(return_reg, regs); 
}

private
int is_hw_irq(unsigned int irq) {
    return (irq == INT_SYSTIME || irq == INT_KEYBOARD);
}

public
void interrupt_register(unsigned int irq, void (*isr)(unsigned int* return_reg, struct cpu_state*)) {
    int_handler_table[irq] = isr;
    if (is_hw_irq(irq)) {
        _dbg_log("Enabling HW interrupt %u.\n", irq);
        pic_enable_irq(irq);
    }
}

public
void interrupt_init(void) {
    pic_init();         // Programmable interrupt controller

    /*
    IRQ 0 ‒ system timer
    IRQ 1 — keyboard controller
    IRQ 3 — serial port COM2
    IRQ 4 — serial port COM1
    IRQ 5 — line print terminal 2
    IRQ 6 — floppy controller
    IRQ 7 — line print terminal 1
    IRQ 8 — RTC timer
    IRQ 12 — mouse controller
    IRQ 13 — math co-processor
    IRQ 14 — ATA channel 1
    IRQ 15 — ATA channel 2
    */

    // Init ISR table
    int_handler_table[INT_KEYBOARD] = ISR_KEYBOARD;
    int_handler_table[INT_PAGEFAULT] = ISR_PAGEFAULT;
    int_handler_table[INT_SYSCALL] = ISR_SYSCALL;

    // Keyboard press interrupt, 0x20 + 1 (which is PIC1_START_INTERRUPT + IRQ_1)
    interrupt_encode_idt_entry(INT_SYSTIME, (unsigned int)asm_int_handler_32);
    interrupt_encode_idt_entry(INT_KEYBOARD, (unsigned int)asm_int_handler_33);
    interrupt_encode_idt_entry(INT_PAGEFAULT, (unsigned int)asm_int_handler_14);
    interrupt_encode_idt_entry(INT_SYSCALL, (unsigned int)asm_int_handler_128);

    IDT.size = sizeof(struct idt_entry) * IDT_SIZE - 1;
    IDT.address = (unsigned int)idt_entries;

    lidt(&IDT);  // ASM wrapper, load interrupt table

    // Tell PIC to receive external hardware interrupts
    pic_enable_irq(INT_KEYBOARD);
}

public
void interrupt_handler(unsigned int* return_reg, struct cpu_state cpu_state, unsigned int interrupt_num, struct stack_state stack_state) {
    //_dbg_log("[Interrupt]num:[%u], eax:[%x]\n", interrupt_num, cpu_state.eax);

    if (interrupt_num >= sizeof(int_handler_table) / sizeof(*int_handler_table)) {
        _dbg_log("Error. Unknown interrupt number.\n");
        return;  // Stop if array out of range
    }

    if (int_handler_table[interrupt_num]) {
        (*int_handler_table[interrupt_num])(return_reg, &cpu_state);
        pic_ack(interrupt_num);
    }
}
