#include "pic.h"
#include "io.h"

#define PIC1_PORT_A 0x20
#define PIC2_PORT_A 0xa0
#define PIC1_COMMAND	PIC1_PORT_A
#define PIC1_DATA	    (PIC1_PORT_A + 1)
#define PIC2_COMMAND	PIC2_PORT_A
#define PIC2_DATA	    (PIC2_PORT_A + 1)

#define PIC1_START_INTERRUPT 0x20
#define PIC2_START_INTERRUPT 0x28
#define PIC2_END_INTERRUPT PIC2_START_INTERRUPT + 7

#define PIC_ACK 0x20

/* reinitialize the PIC controllers, giving them specified vector offsets
rather than 8h and 70h, as configured by default */
 
#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */
 
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */
 
/*
From OSdev - PIC
Arguments:
    offset1 - vector offset for master PIC, vectors on the master become offset1..offset1+7
    offset2 - same for slave PIC: offset2..offset2+7
This will remap all hw interrupt numbers to offset + IRQ
*/
void pic_remap(int offset1, int offset2) {
    outb(PIC1_COMMAND, ICW1_INIT + ICW1_ICW4);	// starts the initialization sequence (in cascade mode)
    outb(PIC2_COMMAND, ICW1_INIT + ICW1_ICW4);
    outb(PIC1_DATA, offset1);				// ICW2: Master PIC vector offset
    outb(PIC2_DATA, offset2);				// ICW2: Slave PIC vector offset
    outb(PIC1_DATA, 4);					    // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    outb(PIC2_DATA, 2);					    // ICW3: tell Slave PIC its cascade identity (0000 0010)

    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);
}

void pic_init() {
    pic_remap(PIC1_START_INTERRUPT, PIC2_START_INTERRUPT);
 
    // IMR
    outb(PIC1_DATA, 0b11111101); // IRQ1 for keyboard interrupt
    outb(PIC2_DATA, 0b11111111);

    asm("sti"); // ENable interrupts
}

// Send acknowledge byte back to PIC, otherwise it will stop generating interrupts.
void pic_ack(unsigned int pic_int_num) {
    if (pic_int_num < PIC1_START_INTERRUPT || pic_int_num > PIC2_END_INTERRUPT) {
        return;
    }

    if (pic_int_num < PIC2_START_INTERRUPT) {
        outb(PIC1_PORT_A, PIC_ACK);
    } else {
        outb(PIC2_PORT_A, PIC_ACK);
    }
}

// https://en.wikibooks.org/wiki/X86_Assembly/Programmable_Interrupt_Controller