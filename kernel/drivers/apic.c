#include "stdint.h"
#include "stddef.h"
#include "drivers/acpi/madt.h"
#include "cpu.h"
#include "utils/debug.h"
#include "builddef.h"


#define APIC_LOCAL_APIC_VERSION 0x0030           // 32 bits reg
#define APIC_LOCAL_APIC_VERSION_MAX_LVT_ENTRY 16 // 8 bits val

#define APIC_LOCAL_APIC_ID 0x0020
#define APIC_LOCAL_APIC_ID_BIT 24

#define APIC_SPURIOUS_INT_VECTOR 0x00f0         // 32 bits reg
#define APIC_SPURIOUS_INT_VECTOR_EN (1 << 8)    // 1 bit val

private
int32_t has_apic() {
    int32_t ret = cpuid_hasapic();
    if (!ret) {
        _dbg_log("No APIC.\n");
    }
    return ret;
}

public
uint8_t apic_get_ver(size_t apic_base) {
    uint32_t apic_ver_reg = *(uint32_t*)(apic_base + APIC_LOCAL_APIC_VERSION);
    uint8_t apic_ver = ((apic_ver_reg & 0xff) >= 0x14);
    uint8_t apic_max_lvt = (apic_ver_reg >> APIC_LOCAL_APIC_VERSION_MAX_LVT_ENTRY) & 0xff;

    _dbg_log("[APIC]Version %u, Max LVT entry [%u]\n", apic_ver, apic_max_lvt);
    return apic_ver;
}

public
uint8_t apic_get_id(size_t apic_base) {
    uint32_t apic_id_reg = *(uint32_t*)(apic_base + APIC_LOCAL_APIC_ID);
    uint8_t apic_id = (apic_id_reg >> APIC_LOCAL_APIC_ID_BIT) & 0xf;
    return apic_id;
}

public
int32_t apic_enable(size_t apic_base) {
    if (!has_apic()) {
        return -1;
    }
    apic_get_ver(apic_base);
    _dbg_log("[APIC]Current ID[%u]\n", apic_get_id(apic_base));
    *(uint32_t*)(apic_base + APIC_SPURIOUS_INT_VECTOR) |= APIC_SPURIOUS_INT_VECTOR_EN;
    return 0;
}
