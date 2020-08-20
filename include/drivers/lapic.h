#ifndef INCLUDE_LAPIC_H
#define INCLUDE_LAPIC_H
#include "stdint.h"
#include "stddef.h"
int32_t lapic_enable(size_t apic_base);
void lapic_send_init(size_t lapic_base, uint8_t lapic_id);
void lapic_send_startup(size_t lapic_base, uint8_t lapic_id, size_t vector);
#endif
