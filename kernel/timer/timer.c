#include "builddef.h"
#include "timer.h"
#include "interrupt.h"
#include "pit.h"
#include "utils/debug.h"

// 0x20 - Programmable Interval Timer
private
void ISR_SYSTIME(unsigned int* return_reg, struct cpu_state* unused) {
    static unsigned int counter;
    counter++;
    if (counter % 1000 == 0) {
        _dbg_log("[Timer]Time passed: %ums\n", counter);
    }
}

public
int timer_init() {
    pit_setfreq(1000);
    interrupt_register(INT_SYSTIME, ISR_SYSTIME);
    return 0;
}
