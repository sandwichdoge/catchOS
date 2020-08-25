
#include "builddef.h"
#include "drivers/pit.h"
#include "interrupt.h"
#include "tasks.h"
#include "utils/debug.h"

static size_t _ticks;
static size_t _freq;

// 0x20 - Programmable Interval Timer. Used for bootstrapping (before scheduler init).
private
void ISR_SYSTIME_BOOTSTRAP(size_t* return_reg, struct cpu_state* unused) {
    _ticks++;
}

// Used for scheduler. This function calls scheduler.
private
void ISR_SYSTIME_SCHED(size_t* return_reg, struct cpu_state* unused) {
    _ticks++;
    task_isr_priority();
}

public
size_t getticks() { return _ticks; }

// After scheduler init
public
void delay(size_t ms) {
    size_t ticks_to_wait = (ms * _freq) / 1000;
    size_t stop = _ticks + ticks_to_wait;
    while (_ticks < stop) {
        task_yield();
    }
}

// Before scheduler init.
public
void delay_bootstrap(size_t ms) {
    size_t ticks_to_wait = (ms * _freq) / 1000;
    size_t stop = _ticks + ticks_to_wait;
    while (_ticks < stop) {
        asm("hlt");
    }
}

// Init timer to be used to bootstrap time-sensitive hardware (e.g. SMP).
public
int32_t timer_init_bootstrap(size_t freq) {
    _freq = freq;
    pit_setfreq(freq);
    interrupt_register(INT_SYSTIME, ISR_SYSTIME_BOOTSTRAP);
    return 0;
}

// Init timer to be used for scheduler.
public
int32_t timer_init_sched(size_t freq) {
    _freq = freq;
    pit_setfreq(freq);
    interrupt_register(INT_SYSTIME, ISR_SYSTIME_SCHED);
    return 0;
}
