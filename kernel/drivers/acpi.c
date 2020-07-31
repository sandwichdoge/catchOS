#include "kinfo.h"
#include "stdint.h"
#include "builddef.h"
#include "pageframe_alloc.h"
#include "paging.h"
#include "utils/debug.h"

struct RSDPDescriptor {
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    uint32_t RsdtAddress;
} __attribute__ ((packed));

struct RSDPDescriptor20 {
    struct RSDPDescriptor firstPart;
    
    uint32_t Length;
    uint64_t XsdtAddress;
    uint8_t ExtendedChecksum;
    uint8_t reserved[3];
} __attribute__ ((packed));

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
    uint32_t *others;
};

static struct RSDT *_rsdt;

void traverse_SDT(int is_paging_enabled) {
    int entries = (_rsdt->h.Length - sizeof(struct ACPISDTHeader)) / 4;
    _dbg_log("SDT entries:%d..\n", entries);
    for (int i = 0; i < entries; ++i) {
        uint32_t* tail = &_rsdt->others;
        tail += i;
        struct ACPISDTHeader *sdt = *tail;
        if (is_paging_enabled) {
            pageframe_set_page_from_addr((void*)sdt, 1);
            paging_map_page(sdt, sdt, get_kernel_pd());
        }
        _dbg_log("i[%d],sdt[0x%x], signature[%s]\n", i, sdt, sdt->Signature);
    }
}

public void acpi_init(int is_paging_enabled) {
    struct kinfo* kinfo = get_kernel_info();
    int acpi_ver = kinfo->acpi_ver;
    struct RSDPDescriptor *rsdp;
    if (acpi_ver == 1) {
        rsdp = (struct RSDPDescriptor *)kinfo->rsdp;
    } else {
        rsdp = &((struct RSDPDescriptor20 *)kinfo->rsdp)->firstPart;
    }
    _dbg_log("ACPI detected, RSDP at [0x%x], signature[%s], OEMID[%s], RSDT at[0x%x]\n", rsdp, rsdp->Signature, rsdp->OEMID, rsdp->RsdtAddress);
    // TODO validate checksum
    _rsdt = (struct RSDT*)rsdp->RsdtAddress;
    if (is_paging_enabled) {
        pageframe_set_page_from_addr((void*)_rsdt, 1);
        paging_map_page(_rsdt, _rsdt, get_kernel_pd());
    }

    traverse_SDT(is_paging_enabled);
    _dbg_break();
}
