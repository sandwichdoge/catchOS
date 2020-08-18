#include "drivers/acpi/madt.h"
#include "utils/debug.h"
#include "builddef.h"

//http://www.osdever.net/tutorials/view/multiprocessing-support-for-hobby-oses-explained

#define ACPI_MADT_POLARITY_CONFORMS       0
#define ACPI_MADT_POLARITY_ACTIVE_HIGH    1
#define ACPI_MADT_POLARITY_RESERVED       2
#define ACPI_MADT_POLARITY_ACTIVE_LOW     3

#define ACPI_MADT_TRIGGER_CONFORMS        (0)
#define ACPI_MADT_TRIGGER_EDGE            (1<<2)
#define ACPI_MADT_TRIGGER_RESERVED        (2<<2)
#define ACPI_MADT_TRIGGER_LEVEL           (3<<2)

static struct MADT* _madt = NULL;

struct MADT* acpi_get_madt() {
    if (!_madt) {
        _madt = acpi_get_sdt_from_sig("APIC");
        _dbg_log("Found madt at 0x%x\n", _madt);
    }
    return _madt;
}

void madt_parse(struct MADT* madt) {
    if (!madt) {
        _dbg_log("Invalid madt\n");
        return;
    }
    _dbg_log("Local APIC addr [0x%x]\n", madt->local_apic_addr);
    uint32_t madt_len = madt->h.Length;
    struct MADT_entry_header *entry = (struct MADT_entry_header *)madt->entries;
    while ((char*)entry + entry->entry_len <= (char*)madt + madt_len) {
        _dbg_log("Entry: type [%d], len [%d]\n", entry->entry_type, entry->entry_len);
        if (entry->entry_len == 0) break;
        switch (entry->entry_type) {
            case MADT_ENTRY_TYPE_LOCAL_APIC: {
                struct MADT_entry_processor_local_APIC *local_apic = (struct MADT_entry_processor_local_APIC *)entry;
                _dbg_log("[Local APIC] ProcessorID [%u], ID[%u]\n", local_apic->ACPI_processor_id, local_apic->APIC_id);
                break;
            }
            case MADT_ENTRY_TYPE_IO_APIC: {
                struct MADT_entry_io_APIC *io_apic = (struct MADT_entry_io_APIC *)entry;
                _dbg_log("[IO APIC] ID[%u], addr[0x%x], Global Interrupt Base[0x%x]\n", io_apic->io_APIC_id, io_apic->io_APIC_addr, io_apic->global_system_interrupt_base);
                break;
            }
            case MADT_ENTRY_TYPE_SOURCE_OVERRIDE: {
                struct MADT_interrupt_source_override *source_override = (struct MADT_interrupt_source_override*)entry;
                _dbg_log("[Source Override]Bus Source[%u], IRQ Source[%u], Global System Interrupt[%u], Flags[%u]\n", 
                    source_override->bus_source, source_override->irq_source, source_override->global_system_interrupt, source_override->flags);
                break;
            }
            case MADT_ENTRY_NMI: {
                struct MADT_nonmaskable_interrupts *nmi = (struct MADT_nonmaskable_interrupts*)entry;
                _dbg_log("[NMI]ProcessorId [0x%x], Flags[%u], LINT#[%u]\n", nmi->ACPI_processor_id, nmi->flags, nmi->LINT_no);
                break;
            }
        }
        entry = (char*)entry + entry->entry_len;
    }
}

