#ifndef INCLUDE_MADT_H
#define INCLUDE_MADT_H
#include "drivers/acpi/acpi.h"
#include "stdint.h"

struct MADT_entry_header {
    uint8_t entry_type;
    uint8_t entry_len;
};

// Type 0. This type represents a single physical processor and its local interrupt controller.
struct MADT_entry_processor_local_APIC {
    struct MADT_entry_header h;
    uint8_t ACPI_processor_id;
    uint8_t APIC_id;
    uint32_t flags; // (bit 0 = Processor Enabled) (bit 1 = Online Capable)
};

// Type 1. This type represents a I/O APIC. The global system interrupt base is the first interrupt number that this I/O APIC handles.
// https://wiki.osdev.org/APIC#IO_APIC_Registers
struct MADT_entry_io_APIC {
    struct MADT_entry_header h;
    uint8_t io_APIC_id;
    uint8_t reserved;
    uint32_t io_APIC_addr;
    uint32_t global_system_interrupt_base;
};

// Type 2. This type explains how IRQ sources are mapped to global system interrupts.
struct MADT_interrupt_source_override {
    struct MADT_entry_header h;
    uint8_t bus_source;
    uint8_t irq_source;
    uint32_t global_system_interrupt;
    uint16_t flags;
};

// Type 4. Configure these with the LINT0 and LINT1 entries in the Local vector table of the relevant processor(')s(') local APIC. 
struct MADT_nonmaskable_interrupts {
    struct MADT_entry_header h;
    uint8_t ACPI_processor_id;
    uint16_t flags;
    uint8_t LINT_no;    // 0 or 1
};

// Type 5. Provides 64 bit systems with an override of the physical address of the Local APIC.
// If this structure is defined, the 64-bit Local APIC address stored within it should be used instead of the 32-bit Local APIC address stored in the MADT header.
struct MADT_local_APIC_addr_override {
    struct MADT_entry_header h;
    uint16_t reserved;
    uint64_t local_APIC_phys_addr;
};

struct MADT {
    struct ACPISDTHeader h;
    uint32_t local_apic_addr;
    uint32_t flags; // (1 = Dual 8259 Legacy PICs Installed)
    uint32_t entries[0];
};

#endif
