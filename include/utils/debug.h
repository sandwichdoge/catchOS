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

#define _dbg_save_edi_esi() { \
    asm ("push %esi");        \
    asm ("push %edi");        \
}

#define _dbg_restore_edi_esi() { \
    asm ("pop %edi");            \
    asm ("pop %esi");            \
}

#define _dbg_break() {      \
    asm("xchg %bx, %bx");   \
}
