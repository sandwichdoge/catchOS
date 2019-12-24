ENTRY(loader)

SECTIONS {
    . = 0xC0200000;

    .rodata ALIGN (0x0000): AT(ADDR(.rodata)-0xC0000000)
    {
        *(.multiboot)
        *(.rodata)
    }

    .text ALIGN (0x1000): AT(ADDR(.text)-0xC0000000)
    {
        *(.text)
    }

    .data ALIGN (0x1000): AT(ADDR(.data)-0xC0000000)
    {
        *(.data)
    }
    
    .bss ALIGN (0x1000): AT(ADDR(.bss)-0xC0000000)
    {
        *(COMMON)
        *(.bss)
    }
}

KERNEL_PROGRAM_END = .;