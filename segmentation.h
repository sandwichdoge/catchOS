#ifndef INCLUDE_SEGMENTATION_H
#define INCLUDE_SEGMENTATION_H
struct gdt_entry {
    unsigned int base;
    unsigned int limit;
    unsigned int type;
};

void seg_encode_gdt_entry(char* target, struct gdt_entry source);
void seg_load_gdt(char* target);
void seg_init_gdt();
#endif