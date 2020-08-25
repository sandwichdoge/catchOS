#include "drivers/ioapic.h"
#include "builddef.h"
#include "utils/debug.h"
#include "drivers/acpi/madt.h"
#include "drivers/io.h"

// REDTBL allows us to choose which external interrupts are sent to which processors.
// Index of REDTBL starts from index 0x10, 64 bits per redirection entry (corresponds to 1 irq).
// Write index of destination register in IOREGSEL then data in IOWIN. This is the only way to access IOAPIC.
// Range from 0x10 to 0x3F offset from IOAPIC base.
#define IOAPIC_REGWIN 0x10

#define IOAPIC_EXTERNAL_IRQ_BASE 0x10

union ioapic_redir_entry {
    struct {
        uint64_t vector : 8;          // Interrupt vector (interrupt number) that will be raised on dest CPU. 0x1F and below are reserved!
        uint64_t delivery_mode : 3;   // 000 (Fixed), 001 (Lowest Priority), 010 (SMI), 100 (NMI), 101 (INIT), 111 (ExtINT).
        uint64_t dest_mode : 1;       // Choose physical dest for now.
        uint64_t delivery_stat : 1;   // 1=send pending, 0=relaxed.
        uint64_t pin_polarity : 1;    // 0=active high, 1=active low. Assume active high for ISA IRQs, unless specified in InterruptSourceOverride in MADT.
        uint64_t trigger_irr : 1;
        uint64_t trigger_mode : 1;    // 0=edge, 1=level. Assume Edge for ISA IRQs, unless specified in IntSourceOverride in MADT.
        uint64_t disabled : 1;         // 0=this irq is enabled, 1=disabled, just like legacy PIC.
        uint64_t reserved : 39;
        uint64_t dest : 8;            // ID of the dest APIC. Need to review format if send to logical dest.
    };
    struct {
        uint32_t low;
        uint32_t high;
    };
};

private
void ioapic_write_reg(size_t apic_base, uint32_t reg_index, uint32_t data) {
    _dbg_log("Write to ioapic base [0x%x], reg [0x%x], data[0x%x]\n", apic_base, reg_index, data);
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
void ioapic_redirect_external_int(size_t apic_base, uint8_t irq, uint8_t dest_cpu) {
    union ioapic_redir_entry entry = {0};
    entry.vector = irq + 32;
    entry.delivery_mode = 0;
    entry.dest_mode = 0;    // Phys dest mode
    entry.delivery_stat = 0;
    entry.pin_polarity = 0; // Active-high
    entry.trigger_mode = 0; // Edge-trigg
    entry.disabled = 0;     // Enabled
    entry.dest = dest_cpu;

    ioapic_write_reg(apic_base, IOAPIC_EXTERNAL_IRQ_BASE + (irq * 2), entry.low);
    ioapic_write_reg(apic_base, IOAPIC_EXTERNAL_IRQ_BASE + (irq * 2) + 1, entry.high);
}

public
void ioapic_init(size_t apic_base) {
    // Map irq#n to irq#n + 32 (for all 24 irqs)
    uint32_t ver = ioapic_read_reg(apic_base, 1); // VER
    uint32_t pins = ((ver >> 16) & 0xff) + 1;
    _dbg_log("IOAPIC pins: %d\n", pins);

    for (uint8_t i = 0; i < pins; ++i) {
        ioapic_redirect_external_int(apic_base, i, 0);
    }
}
