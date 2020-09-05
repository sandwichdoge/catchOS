#include "cpu.h"
#include "builddef.h"

// Read current EFLAGS register.
public
size_t get_flags_reg() {
#ifdef TARGET_HOST
    return 0;
#endif
    size_t ret;
    asm("pushf\n"
        "movl (%%esp), %%eax\n"
        "popf\n"
        : "=a"(ret));
    return ret;
}

public
inline void cpu_relax() { asm volatile("rep; nop" ::: "memory"); }
