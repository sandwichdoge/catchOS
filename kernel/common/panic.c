#include "builddef.h"
#include "smp.h"
#include "utils/debug.h"

static inline void cpu_relax() { asm volatile("rep; nop"); }

public
void panic() {
    _dbg_log("[CPU%d]Panic!\n", smp_get_cpu_id());
    while (1) cpu_relax();
}
