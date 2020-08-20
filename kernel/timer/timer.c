
#include "builddef.h"
#include "drivers/pit.h"
#include "interrupt.h"
#include "tasks.h"

#define TICK_FREQ_HZ 1000

private
size_t _ticks;
size_t _freq;

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
    size_t ticks_to_wait = ms * (_freq / 1000);
    size_t stop = _ticks + ticks_to_wait;
    while (_ticks < stop) {
        asm("hlt");
    }
}

public
int32_t timer_init(size_t freq) {
    _freq = freq;
    pit_setfreq(freq);
    interrupt_register(INT_SYSTIME, ISR_SYSTIME);
    return 0;
}
