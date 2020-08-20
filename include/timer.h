#ifndef INCLUDE_TIMER_H
#define INCLUDE_TIMER_H
#include "stddef.h"
#include "stdint.h"

int32_t timer_init(size_t freq);
// Get clock tick count from PIT.
size_t getticks();
void delay(size_t ms);
#endif
