#include "builddef.h"
#include "timer.h"
#include "interrupt.h"
#include "drivers/pit.h"
#include "utils/debug.h"

#define TICK_FREQ_HZ 100

private unsigned int _ticks;

// 0x20 - Programmable Interval Timer
private
void ISR_SYSTIME(unsigned int* return_reg, struct cpu_state* unused) {
    _ticks++;
}

public
unsigned int getticks() {
    return _ticks;
}

public
void delay(unsigned int ms) {
    unsigned int stop = _ticks * 10 + ms;
    while (_ticks * 10 < stop) {
    }
}

public
int timer_init() {
    pit_setfreq(TICK_FREQ_HZ);
    interrupt_register(INT_SYSTIME, ISR_SYSTIME);
    return 0;
}
