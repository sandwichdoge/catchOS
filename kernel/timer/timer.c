
#include "builddef.h"
#include "drivers/pit.h"
#include "interrupt.h"
#include "tasks.h"

#define TICK_FREQ_HZ 100

private
size_t _ticks;

// 0x20 - Programmable Interval Timer
private
void ISR_SYSTIME(size_t* return_reg, struct cpu_state* unused) {
    _ticks++;
    task_isr_priority();
}

public
size_t getticks() { return _ticks; }

public
void delay(size_t ms) {
    size_t stop = _ticks * 10 + ms;
    while (_ticks * 10 < stop) {
        task_yield();
    }
}

public
int32_t timer_init() {
    pit_setfreq(TICK_FREQ_HZ);
    interrupt_register(INT_SYSTIME, ISR_SYSTIME);
    return 0;
}
