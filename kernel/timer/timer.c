#include "builddef.h"
#include "timer.h"
#include "interrupt.h"
#include "drivers/pit.h"
#include "utils/debug.h"

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
int timer_init() {
    pit_setfreq(1000);
    interrupt_register(INT_SYSTIME, ISR_SYSTIME);
    return 0;
}
