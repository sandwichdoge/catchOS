#ifndef INCLUDE_LAPIC_H
#define INCLUDE_LAPIC_H
#include "stddef.h"
#include "stdint.h"
void lapic_send_init(size_t lapic_base, uint8_t lapic_id);
void lapic_send_startup(size_t lapic_base, uint8_t lapic_id, size_t vector);
uint8_t lapic_get_id(size_t lapic_base);
int32_t lapic_init(size_t lapic_base);
void lapic_ack();
size_t lapic_get_base();
#endif
