#ifndef INCLUDE_IO_APIC_H
#define INCLUDE_IO_APIC_H
#include "stddef.h"
#include "stdint.h"
void ioapic_init(size_t apic_base);
void ioapic_redirect_external_int(size_t apic_base, uint8_t irq, uint8_t dest_cpu);
#endif
