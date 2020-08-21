#include "drivers/ioapic.h"
#include "builddef.h"

#define IOAPIC_REDTBL_L_VECTOR 0    // 8bits
#define IOAPIC_REDTBL_L_DELMOD 8    // 2bits
#define IOAPIC_REDTBL_L_DESMOD 11   // 1bit, choose physical dest for now.
#define IOAPIC_REDTBL_L_DELSTAT 12  // 1bit
#define IOAPIC_REDTBL_L_PINPOLAR 13 // 1bit, 0=active high, 1=active low, assume active high for ISA IRQs
#define IOAPIC_REDTBL_L_TRIGMOD 14  // 1bit
#define IOAPIC_REDTBL_L_MASK 16     // 1=this irq is enabled, 0=disabled, just like legacy PIC

#define IOAPIC_REDTBL_H_DEST 24     // 8bit, ID of the dest APIC

// Index of REDTBL starts from index 0x10, 64 bits per redirection entry (corresponds to 1 irq).
// Write index of destination register in IOREGSEL then data in IOWIN. This is the only way to access IOAPIC.
#define IOAPIC_REGWIN 0x10

union ioapic_redir_entry_u {
    struct ioapic_redir_entry {
        size_t vector : 8;
        size_t delivery_mode : 2;
        size_t dest_mode : 1;
        size_t delivery_stat : 1;
        size_t pin_polarity : 1;
        size_t trigger_irr : 1;
        size_t trigger_mode : 1;
        size_t enabled : 1;
        size_t reserved : 39;
        size_t dest : 8;
    };
    uint32_t low;
    uint32_t high;
};

private
void ioapic_write_reg(size_t apic_base, uint32_t reg_index, uint32_t data) {
    volatile uint32_t* io_reg_sel = (volatile uint32_t*)apic_base;
    volatile uint32_t* io_reg_win = (volatile uint32_t*)(apic_base + IOAPIC_REGWIN);
    *io_reg_sel = reg_index;
    *io_reg_win = data;
}

private
uint32_t ioapic_read_reg(size_t apic_base, uint32_t reg_index) {
    volatile uint32_t* io_reg_sel = (volatile uint32_t*)apic_base;
    volatile uint32_t* io_reg_win = (volatile uint32_t*)(apic_base + IOAPIC_REGWIN);
    *io_reg_sel = reg_index;
    return *io_reg_win;
}
