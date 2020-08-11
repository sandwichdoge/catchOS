#include "builddef.h"
#include "utils/debug.h"

static inline void cpu_relax() {
    asm volatile("rep; nop");
}

public
void panic() {
    _dbg_log("Panic!\n");    
    while (1)   
        cpu_relax();
}
