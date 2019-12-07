#include "segmentation.h"
#include "gdt.h" // ASM wrapper for lgdt

// 3 GDT entries: Null descriptor, kernel code segment, kernel data segment.
struct gdt_entry GDT[3] = {0};

void seg_encode_gdt_entry(int index, unsigned int base_address, unsigned int limit, unsigned char type, unsigned char flags) {
        GDT[index].base_low = base_address & 0xFFFF;
        GDT[index].base_middle = (base_address >> 16) & 0xFF;
        GDT[index].base_high = (base_address >> 24) & 0xFF;

        GDT[index].limit_low = limit & 0xFFFF;
        GDT[index].limit_and_flags = (limit >> 16) & 0xF;
        GDT[index].limit_and_flags |= (flags << 4) & 0xF0;

        GDT[index].type = type;
}


// Init GDT for 4 GiB memory
void seg_init_gdt() {
    // Init null descriptor (already nulled out)

    // Init kernel code segment
    seg_encode_gdt_entry(1, 0x0, 0xfffff, 0x9a, 0xc);
    seg_encode_gdt_entry(2, 0x0, 0xfffff, 0x92, 0xc);

    struct gdt asm_lgdt_arg;
    asm_lgdt_arg.address = (unsigned int)GDT;
    asm_lgdt_arg.size = sizeof(GDT);
    lgdt(&asm_lgdt_arg);
    lsr();
}