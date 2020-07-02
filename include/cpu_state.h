#ifndef INCLUDE_CPU_STATE_H
#define INCLUDE_CPU_STATE_H

struct cpu_state {
    unsigned int edi;
    unsigned int esi;
    unsigned int ebp;
    unsigned int edx;
    unsigned int ecx;
    unsigned int ebx;
    unsigned int eax;
} __attribute__((packed));

struct stack_state {
    unsigned int error_code;
    unsigned int eip;
    unsigned int cs;
    unsigned int eflags;
} __attribute__((packed));

#endif
