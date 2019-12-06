#include "segmentation.h"
#include "gdt.h" // ASM wrapper for lgdt

// https://wiki.osdev.org/GDT_Tutorial
void seg_encode_gdt_entry(char* target, struct gdt_entry source) {
    // Check the limit to make sure that it can be encoded
    if ((source.limit > 65536) && ((source.limit & 0xFFF) != 0xFFF)) {
        //kerror("You can't do that!");
    }
    if (source.limit > 65536) {
        // Adjust granularity if required
        source.limit = source.limit >> 12;
        target[6] = 0xC0;
    } else {
        target[6] = 0x40;
    }
 
    // Encode the limit
    target[0] = source.limit & 0xFF;
    target[1] = (source.limit >> 8) & 0xFF;
    target[6] |= (source.limit >> 16) & 0xF;
 
    // Encode the base 
    target[2] = source.base & 0xFF;
    target[3] = (source.base >> 8) & 0xFF;
    target[4] = (source.base >> 16) & 0xFF;
    target[7] = (source.base >> 24) & 0xFF;
 
    // And... Type
    target[5] = source.type;
}

char GDT[8 * 3] = {0}; // 3 GDT entries: Null descriptor, kernel code segment, kernel data segment.

// Init GDT for 4 GiB memory
void seg_init_gdt() {
    // Init null descriptor
    for (unsigned char i = 0; i < 8; i++) {
        GDT[i] = 0;
    }

    // Init kernel code segment
    struct gdt_entry kcode_desc;
    kcode_desc.base = 0x04000000;
    kcode_desc.limit = 0x03ffffff;
    kcode_desc.type = 0x9A; // Type code
    
    seg_encode_gdt_entry((char*)(GDT + 8), kcode_desc);

    // Init kernel data segment
    struct gdt_entry kdata_desc;
    kdata_desc.base = 0x08000000;
    kdata_desc.limit = 0x03ffffff;
    kdata_desc.type = 0x92; // Type data

    seg_encode_gdt_entry((char*)(GDT + 16), kdata_desc);

    struct gdt asm_lgdt_arg;
    asm_lgdt_arg.address = (unsigned int)GDT;
    asm_lgdt_arg.size = sizeof(GDT);
    lgdt(&asm_lgdt_arg);
    lsr();
}