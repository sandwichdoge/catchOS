#ifndef INCLUDE_INTERRUPT_H
#define INCLUDE_INTERRUPT_H
#include "cpu_state.h"

#define INT_GPF 13
#define INT_PAGEFAULT 14
#define INT_SYSTIME 32
#define INT_KEYBOARD 33              // 0x20 + 1
#define INT_COM1 36                  // 0x20 + 4
#define INT_DEVICE_NOT_AVAiLABLE 39  // 0x27
#define INT_SYSCALL 128

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

void interrupt_init(void);
void interrupt_register(unsigned int irq, void (*isr)(unsigned int* return_reg, struct cpu_state*));
#endif
