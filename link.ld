ENTRY(loader)

SECTIONS {
    . = 0x00200000;

    .rodata ALIGN (0x0000): AT(ADDR(.rodata)-0x00000000)
    {
        *(.multiboot)
        *(.rodata)
    }

    .text ALIGN (0x1000): AT(ADDR(.text)-0x00000000)
    {
        *(.text)
    }

    .data ALIGN (0x1000): AT(ADDR(.data)-0x00000000)
    {
        *(.data)
    }
    
    .bss ALIGN (0x1000): AT(ADDR(.bss)-0x00000000)
    {
        *(COMMON)
        *(.bss)
    }
}

KERNEL_PROGRAM_END = .;