#ifndef INCLUDE_TIMER_H
#define INCLUDE_TIMER_H
#include "stddef.h"
#include "stdint.h"

int32_t timer_init_bootstrap(size_t freq);
int32_t timer_init_sched(size_t freq);
// Get clock tick count from PIT.
size_t getticks();

// Normal sleep. Will yield cpu control to other tasks.
void delay(size_t ms);

// Only call this before the scheduler is initialized.
void delay_bootstrap(size_t ms);

// Real-time sleep. This function will never willingly give up cpu control to other tasks.
// Only use this for short sleeps when it's absolutely necessary.
void delay_rt(size_t ms);
#endif
