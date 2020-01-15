#ifndef INCLUDE_DEBUG_H
#define INCLUDE_DEBUG_H
#include "serial.h"
#include "utils/string.h"
#define _dbg_set_edi_esi(val) {     \
    unsigned int tmp;               \
                                    \
    asm ("movl %1, %%esi;"          \
    :"=r"(tmp)        /* output */  \
    :"r"(val)         /* input */   \
    :"%esi"         /* clobbered register */ \
    );                              \
    \
    asm ("movl %1, %%edi;"          \
    :"=r"(tmp)        /* output */  \
    :"r"(val)         /* input */   \
    :"%edi"         /* clobbered register */ \
    );                              \
}

#define _dbg_set_esi(val) {     \
    unsigned int tmp;               \
                                    \
    asm ("movl %1, %%esi;"          \
    :"=r"(tmp)        /* output */  \
    :"r"(val)         /* input */   \
    :"%esi"         /* clobbered register */ \
    );                              \
}

#define _dbg_set_edi(val) {     \
    unsigned int tmp;               \
                                    \
    asm ("movl %1, %%edi;"          \
    :"=r"(tmp)        /* output */  \
    :"r"(val)         /* input */   \
    :"%edi"         /* clobbered register */ \
    );                              \
}

#define _dbg_save_edi_esi() { \
    asm ("push %esi");        \
    asm ("push %edi");        \
}

#define _dbg_restore_edi_esi() { \
    asm ("pop %edi");            \
    asm ("pop %esi");            \
}

#ifdef TARGET_BOCHS
#define _dbg_break() {      \
    asm("xchg %bx, %bx");   \
}
#endif // TARGET_BOCHS

#define _dbg_serial(str) { \
    serial_write(SERIAL_COM1_BASE, str, _strlen(str)); \
}


#endif