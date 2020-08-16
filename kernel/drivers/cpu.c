#include "builddef.h"
#include "cpu.h"

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
