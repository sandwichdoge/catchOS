#include "drivers/ioapic.h"
#include "builddef.h"

// REDTBL allows us to choose which external interrupts are sent to which processors.
// Index of REDTBL starts from index 0x10, 64 bits per redirection entry (corresponds to 1 irq).
// Write index of destination register in IOREGSEL then data in IOWIN. This is the only way to access IOAPIC.
// Range from 0x10 to 0x3F offset from IOAPIC base.
#define IOAPIC_REGWIN 0x10
#define IOAPIC_EXTERNAL_IRQ_MAX 24

union ioapic_redir_entry {
    struct {
        uint64_t vector : 8;          // Interrupt vector (interrupt number) that will be raised on dest CPU. 0x1F and below are reserved!
        uint64_t delivery_mode : 2;   // 000 (Fixed), 001 (Lowest Priority), 010 (SMI), 100 (NMI), 101 (INIT), 111 (ExtINT).
        uint64_t dest_mode : 1;       // Choose physical dest for now.
        uint64_t delivery_stat : 1;   // 1=send pending, 0=relaxed.
        uint64_t pin_polarity : 1;    // 0=active high, 1=active low. Assume active high for ISA IRQs, unless specified in InterruptSourceOverride in MADT.
        uint64_t trigger_irr : 1;
        uint64_t trigger_mode : 1;    // 0=edge, 1=level. Assume Edge for ISA IRQs, unless specified in IntSourceOverride in MADT.
        uint64_t enabled : 1;         // 0=this irq is enabled, 1=disabled, just like legacy PIC.
        uint64_t reserved : 39;
        uint64_t dest : 8;            // ID of the dest APIC. Need to review format if send to logical dest.
    };
    uint32_t low;
    uint32_t high;
};

private
void ioapic_write_reg(size_t apic_base, uint32_t reg_index, uint32_t data) {
    uint32_t volatile* io_reg_sel = (uint32_t volatile*)apic_base;
    uint32_t volatile* io_reg_win = (uint32_t volatile*)(apic_base + IOAPIC_REGWIN);
    *io_reg_sel = reg_index;
    *io_reg_win = data;
}

private
uint32_t ioapic_read_reg(size_t apic_base, uint32_t reg_index) {
    uint32_t volatile* io_reg_sel = (uint32_t volatile*)apic_base;
    uint32_t volatile* io_reg_win = (uint32_t volatile*)(apic_base + IOAPIC_REGWIN);
    *io_reg_sel = reg_index;
    return *io_reg_win;
}

public
void ioapic_redirect_external_int(uint8_t irq, uint8_t dest_cpu) {
    union ioapic_redir_entry entry;
    entry.vector = irq;
}

public
void ioapic_init(size_t apic_base) {
    // Map irq#n to irq#n + 32 (for all 24 irqs)
    
}
