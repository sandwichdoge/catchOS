#ifndef INCLUDE_SEGMENTATION_H
#define INCLUDE_SEGMENTATION_H

struct gdt_entry
{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char type;
    unsigned char limit_and_flags;
    unsigned char base_high;
} __attribute__((packed));

void seg_encode_gdt_entry(int index, unsigned int base_address, unsigned int limit, unsigned char type, unsigned char flags);
void seg_load_gdt(char* target);
void seg_init_gdt();
#endif