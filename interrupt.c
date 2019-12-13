#include "interrupt.h"
#include "pic.h"
#include "keyboard.h"
#include "debug.h"

extern void int_handler_33(); // Handler for keyboard press

#define IDT_SIZE 256
#define INT_KEYBOARD 33 // 0x20 + 1

struct cpu_state {
    unsigned int eax;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;
    unsigned int ebp;
    unsigned int esi;
    unsigned int edi;
};

struct stack_state {
    unsigned int error_code;
    unsigned int eip;
    unsigned int cs;
    unsigned int eflags;
};

struct idt_entry idt_entries[IDT_SIZE] = {0};

// https://wiki.osdev.org/Interrupt_Descriptor_Table
void interrupt_encode_idt_entry(unsigned int interrupt_num, unsigned int f_ptr_handler) {
    idt_entries[interrupt_num].offset_low = f_ptr_handler & 0xffff;
    idt_entries[interrupt_num].offset_high = (f_ptr_handler >> 16) & 0xffff;

    idt_entries[interrupt_num].segment_selector = 0x8; // code segment in gdt
    idt_entries[interrupt_num].reserved = 0x0;

    idt_entries[interrupt_num].type_and_attr = (1 << 7) | // P
                                                (0 << 6) | (0 << 5) |	// DPL
                                                (0 << 4) | // S
                                                0b1110;   // 32-bit interrupt gate
}

void lidt (struct idt *idt_r)
{
    asm ("lidt %0" :: "m"(*idt_r));
}

void interrupt_init_idt() {
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

    // Keyboard press interrupt, 0x20 + 1 (which is PIC1_START_INTERRUPT + IRQ_1)
    interrupt_encode_idt_entry(INT_KEYBOARD, (unsigned int)int_handler_33);
    
    struct idt IDT;
    IDT.size = sizeof(struct idt_entry) * IDT_SIZE - 1;
    IDT.address = (unsigned int)idt_entries;

    lidt(&IDT); // ASM wrapper
}

void interrupt_handler(struct cpu_state cpu_state, unsigned int interrupt_num, struct stack_state stack_state) {
    // TODO handle keypress
    switch (interrupt_num) {
        case INT_KEYBOARD: {
            unsigned char scan_code = read_scan_code();
            _dbg_save_edi_esi();
            _dbg_set_edi_esi((unsigned int)scan_code);
            _dbg_break();
            _dbg_restore_edi_esi();
            pic_ack(interrupt_num);
            break;   
        }
        default:
            break;
    }
}