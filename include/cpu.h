#ifndef INCLUDE_CPU_H
#define INCLUDE_CPU_H
#include "stddef.h"
#include "drivers/cpuid.h"

#define CPU_EFLAGS_IF (1 << 9)

size_t get_flags_reg();

#endif
