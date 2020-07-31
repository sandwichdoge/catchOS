#ifndef INCLUDE_ACPI_H
#define INCLUDE_ACPI_H
#include "stdint.h"

struct ACPISDTHeader {  // 36 bytes size
    char Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
} __attribute__ ((packed));

struct RSDT {
    struct ACPISDTHeader h;
    uint32_t others;
};

struct XSDT {
    struct ACPISDTHeader h;
    uint64_t others;
};

struct XSDT* acpi_get_xsdt();
struct RSDT* acpi_get_rsdt();
void acpi_init();
int acpi_get_ver();

#endif