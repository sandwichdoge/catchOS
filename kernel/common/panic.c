#include "builddef.h"
#include "utils/debug.h"
#include "drivers/lapic.h"

static inline void cpu_relax() { asm volatile("rep; nop"); }

public
void panic() {
    _dbg_log("[CPU%d]Panic!\n", lapic_get_cpu_id());
    while (1) cpu_relax();
}
