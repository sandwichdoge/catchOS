// https://wiki.osdev.org/CPUID
#include "drivers/cpuid.h"

#include "builddef.h"
#include "stdint.h"
#include "utils/string.h"

enum cpuid_requests {
    CPUID_GETVENDORSTRING,
    CPUID_GETFEATURES,
    CPUID_GETTLB,
    CPUID_GETSERIAL,

    CPUID_INTELEXTENDED = 0x80000000,
    CPUID_INTELFEATURES,
    CPUID_INTELBRANDSTRING,
    CPUID_INTELBRANDSTRINGMORE,
    CPUID_INTELBRANDSTRINGEND,
};

/** issue a single request to CPUID. Fits 'intel features', for instance
 *  note that even if only "eax" and "edx" are of interest, other registers
 *  will be modified by the operation, so we need to tell the compiler about it.
 */
private
void cpuid(int code, unsigned int *a, unsigned int *d) { asm volatile("cpuid" : "=a"(*a), "=d"(*d) : "a"(code) : "ecx", "ebx"); }

/** issue a complete request, storing general registers output as a string
 */
private
int cpuid_string(int code, unsigned int where[4]) {
    asm volatile("cpuid" : "=a"(*where), "=b"(*(where + 1)), "=c"(*(where + 2)), "=d"(*(where + 3)) : "a"(code));
    return (int)where[0];
}

public
unsigned int cpuid_getcpufeatures(unsigned int *edx_out) {
    unsigned int eax;
    cpuid(CPUID_GETFEATURES, &eax, edx_out);
    return eax;
}

public
int cpuid_hasapic() {
    unsigned int edx = 0;
    cpuid_getcpufeatures(&edx);
    return (edx & CPUID_FEAT_EDX_APIC) != 0;
}

public
void cpuid_getvendor(char *out) {
    unsigned int vendor[4];
    _memset((char *)vendor, 0, 4 * sizeof(unsigned int));
    cpuid_string(CPUID_GETVENDORSTRING, vendor);
    out[0] = vendor[0];
    out[1] = vendor[1];
    out[2] = vendor[2];
    out[3] = vendor[3];
}
