#include "drivers/lapic.h"
#include "drivers/acpi/madt.h"
#include "cpu.h"
#include "utils/debug.h"
#include "builddef.h"

#define LAPIC_ID                        0x0020  // Local APIC ID
#define LAPIC_VER                       0x0030  // Local APIC Version
#define LAPIC_TPR                       0x0080  // Task Priority
#define LAPIC_APR                       0x0090  // Arbitration Priority
#define LAPIC_PPR                       0x00a0  // Processor Priority
#define LAPIC_EOI                       0x00b0  // EOI
#define LAPIC_RRD                       0x00c0  // Remote Read
#define LAPIC_LDR                       0x00d0  // Logical Destination
#define LAPIC_DFR                       0x00e0  // Destination Format
#define LAPIC_SVR                       0x00f0  // Spurious Interrupt Vector
#define LAPIC_ISR                       0x0100  // In-Service (8 registers)
#define LAPIC_TMR                       0x0180  // Trigger Mode (8 registers)
#define LAPIC_IRR                       0x0200  // Interrupt Request (8 registers)
#define LAPIC_ESR                       0x0280  // Error Status
#define LAPIC_ICRLO                     0x0300  // Interrupt Command
#define LAPIC_ICRHI                     0x0310  // Interrupt Command [63:32]
#define LAPIC_TIMER                     0x0320  // LVT Timer
#define LAPIC_THERMAL                   0x0330  // LVT Thermal Sensor
#define LAPIC_PERF                      0x0340  // LVT Performance Counter
#define LAPIC_LINT0                     0x0350  // LVT LINT0
#define LAPIC_LINT1                     0x0360  // LVT LINT1
#define LAPIC_ERROR                     0x0370  // LVT Error
#define LAPIC_TICR                      0x0380  // Initial Count (for Timer)
#define LAPIC_TCCR                      0x0390  // Current Count (for Timer)
#define LAPIC_TDCR                      0x03e0  // Divide Configuration (for Timer)

// ------------------------------------------------------------------------------------------------
// Interrupt Command Register

// Delivery Mode
#define ICR_FIXED                       0x00000000
#define ICR_LOWEST                      0x00000100
#define ICR_SMI                         0x00000200
#define ICR_NMI                         0x00000400
#define ICR_INIT                        0x00000500
#define ICR_STARTUP                     0x00000600

// Destination Mode
#define ICR_PHYSICAL                    0x00000000
#define ICR_LOGICAL                     0x00000800

// Delivery Status
#define ICR_IDLE                        0x00000000
#define ICR_SEND_PENDING                0x00001000

// Level
#define ICR_DEASSERT                    0x00000000
#define ICR_ASSERT                      0x00004000

// Trigger Mode
#define ICR_EDGE                        0x00000000
#define ICR_LEVEL                       0x00008000

// Destination Shorthand
#define ICR_NO_SHORTHAND                0x00000000
#define ICR_SELF                        0x00040000
#define ICR_ALL_INCLUDING_SELF          0x00080000
#define ICR_ALL_EXCLUDING_SELF          0x000c0000

// Destination Field
#define ICR_DESTINATION_SHIFT           24

private
int32_t has_apic() {
    int32_t ret = cpuid_hasapic();
    if (!ret) {
        _dbg_log("No APIC.\n");
    }
    return ret;
}

public
uint8_t lapic_get_ver(size_t lapic_base) {
    uint32_t lapic_ver_reg = *(uint32_t*)(lapic_base + LAPIC_VER);
    uint8_t lapic_ver = ((lapic_ver_reg & 0xff) >= 0x14);
    uint8_t lapic_max_lvt = (lapic_ver_reg >> 16) & 0xff;

    _dbg_log("[APIC]Version %u, Max LVT entry [%u]\n", lapic_ver, lapic_max_lvt);
    return lapic_ver;
}

// Get local CPU's APIC ID
public
uint8_t lapic_get_id(size_t lapic_base) {
    uint32_t lapic_id_reg = *(uint32_t*)(lapic_base + LAPIC_ID);
    uint8_t lapic_id = (lapic_id_reg >> 24) & 0xf;
    return lapic_id;
}

public
void lapic_send_startup(size_t lapic_base, uint8_t lapic_id, size_t vector) {
    uint32_t *lapic_icr_hi = (uint32_t*)(lapic_base + 0x0310);
    uint32_t *lapic_icr_lo = (uint32_t*)(lapic_base + 0x0300);

    *lapic_icr_hi = lapic_id << ICR_DESTINATION_SHIFT;  // DESTINATION
    *lapic_icr_lo = (vector / 4096) | ICR_STARTUP | ICR_PHYSICAL | ICR_ASSERT | ICR_EDGE | ICR_NO_SHORTHAND;

    while (*lapic_icr_lo & ICR_SEND_PENDING)
        ;
    _dbg_log("[LAPIC]Sent SIPI, starting EIP [0x%x].\n", vector);
}

void lapic_send_init(size_t lapic_base, uint8_t lapic_id) {
    uint32_t *lapic_icr_hi = (uint32_t*)(lapic_base + 0x0310);
    uint32_t *lapic_icr_lo = (uint32_t*)(lapic_base + 0x0300);

    *lapic_icr_hi = lapic_id << ICR_DESTINATION_SHIFT;  // DESTINATION
    *lapic_icr_lo = ICR_INIT | ICR_PHYSICAL | ICR_ASSERT | ICR_EDGE | ICR_NO_SHORTHAND;

    while (*lapic_icr_lo & ICR_SEND_PENDING)
        ;
    _dbg_log("[LAPIC]Sent INIT IPI.\n");
}


public
int32_t lapic_enable(size_t lapic_base) {
    if (!has_apic()) {
        return -1;
    }
    lapic_get_ver(lapic_base);
    _dbg_log("[APIC]Current ID[%u]\n", lapic_get_id(lapic_base));
    *(uint32_t*)(lapic_base + LAPIC_SVR) |= 0x100;   // Enable spurious ints
    return 0;
}
