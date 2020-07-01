#include "pic.h"
#include "utils/debug.h"
#include "builddef.h"
#include "io.h"

#define PIC1_PORT_A 0x20
#define PIC2_PORT_A 0xa0
#define PIC1_COMMAND PIC1_PORT_A
#define PIC1_DATA (PIC1_PORT_A + 1)
#define PIC2_COMMAND PIC2_PORT_A
#define PIC2_DATA (PIC2_PORT_A + 1)

#define PIC1_START_INTERRUPT 0x20
#define PIC2_START_INTERRUPT 0x28
#define PIC2_END_INTERRUPT PIC2_START_INTERRUPT + 7

#define PIC_ACK 0x20

/* reinitialize the PIC controllers, giving them specified vector offsets
rather than 8h and 70h, as configured by default */

#define ICW1_ICW4 0x01      /* ICW4 (not) needed */
#define ICW1_SINGLE 0x02    /* Single (cascade) mode */
#define ICW1_INTERVAL4 0x04 /* Call address interval 4 (8) */
#define ICW1_LEVEL 0x08     /* Level triggered (edge) mode */
#define ICW1_INIT 0x10      /* Initialization - required! */

#define ICW4_8086 0x01       /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO 0x02       /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE 0x08  /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C /* Buffered mode/master */
#define ICW4_SFNM 0x10       /* Special fully nested (not) */

private unsigned char pic1, pic2;

/*
From OSdev - PIC
Arguments:
    offset1 - vector offset for master PIC, vectors on the master become offset1..offset1+7
    offset2 - same for slave PIC: offset2..offset2+7
This will remap all hw interrupt numbers to offset + IRQ
*/
public
void pic_remap(int offset1, int offset2) {
    outb(PIC1_COMMAND, ICW1_INIT + ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
    outb(PIC2_COMMAND, ICW1_INIT + ICW1_ICW4);
    outb(PIC1_DATA, offset1);  // ICW2: Master PIC vector offset
    outb(PIC2_DATA, offset2);  // ICW2: Slave PIC vector offset
    outb(PIC1_DATA, 4);        // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    outb(PIC2_DATA, 2);        // ICW3: tell Slave PIC its cascade identity (0000 0010)

    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);
}

public
void pic_init() {
    pic_remap(PIC1_START_INTERRUPT, PIC2_START_INTERRUPT);
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
    // IMR - Default all interrupts disabled.
    pic1 = 0xff;
    pic2 = 0xff;
    outb(PIC1_DATA, pic1);  // (0b11111100) - PIT, keyboard IRQ
    outb(PIC2_DATA, pic2);

    asm("sti");  // Enable interrupts
}

#define PIC_OFFSET 0x20
public
void pic_enable_irq(unsigned int irq_no) {
    irq_no -= PIC_OFFSET;
    if (irq_no < 8) {
        pic1 &= ~(1 << irq_no);
        outb(PIC1_DATA, pic1);
    } else if (irq_no < 16) {
        pic2 &= ~(1 << (irq_no - 8));
        outb(PIC2_DATA, pic2);
    } else {
        _dbg_log("Error. Trying to enable invalid irq: %u.\n", irq_no);
    }
}

// Send acknowledge byte back to PIC, otherwise it will stop generating interrupts.
public
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