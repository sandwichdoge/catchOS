#ifndef INCLUDE_INTERRUPT_H
#define INCLUDE_INTERRUPT_H
#include "cpu_state.h"
#include "stddef.h"
#include "stdint.h"

#define INT_GPF 13
#define INT_PAGEFAULT 14

#define IRQ_REDIR_BASE 32

#define INT_SYSTIME 32
#define INT_KEYBOARD 33              // 0x20 + 1
#define INT_COM1 36                  // 0x20 + 4
#define INT_DEVICE_NOT_AVAiLABLE 39  // 0x27
#define INT_SYSCALL 128

struct idt_entry {
    /* The lowest 32 bits */
    uint16_t offset_low;        // offset bits 0..15
    uint16_t segment_selector;  // a code segment selector in GDT or LDT

    /* The highest 32 bits */
    uint8_t reserved;       // Just 0.
    uint8_t type_and_attr;  // type and attributes
    uint16_t offset_high;   // offset bits 16..31
} __attribute__((packed));

struct idt {
    uint16_t size;
    size_t address;
} __attribute__((packed));

void interrupt_init(void);
void interrupt_register(uint32_t irq, void (*isr)(size_t* return_reg, struct cpu_state*));
struct idt * interrupt_get_idt();
#endif
