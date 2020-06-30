#ifndef INCLUDE_INTERRUPT_H
#define INCLUDE_INTERRUPT_H

struct cpu_state {
    unsigned int edi;
    unsigned int esi;
    unsigned int ebp;
    unsigned int edx;
    unsigned int ecx;
    unsigned int ebx;
    unsigned int eax;
} __attribute__((packed));

struct stack_state {
    unsigned int error_code;
    unsigned int eip;
    unsigned int cs;
    unsigned int eflags;
} __attribute__((packed));

struct idt_entry {
    /* The lowest 32 bits */
    unsigned short offset_low;        // offset bits 0..15
    unsigned short segment_selector;  // a code segment selector in GDT or LDT

    /* The highest 32 bits */
    unsigned char reserved;       // Just 0.
    unsigned char type_and_attr;  // type and attributes
    unsigned short offset_high;   // offset bits 16..31
} __attribute__((packed));

struct idt {
    unsigned short size;
    unsigned int address;
} __attribute__((packed));

void interrupt_init_idt(void);
#endif