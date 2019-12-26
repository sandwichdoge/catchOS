ENTRY(loader)

SECTIONS {
    . = 0xC0200000;

    .multiboot ALIGN (0x4): AT(ADDR(.multiboot)-0xC0000000)
    {
        *(.multiboot)
    }

    .rodata ALIGN (0x1000): AT(ADDR(.rodata)-0xC0000000)
    {
        *(.rodata)
        *(.rodata.*)
    }

    .text ALIGN (0x1000): AT(ADDR(.text)-0xC0000000)
    {
        *(.text)
        *(.text.*)
        *(.text*)
    }

    .data ALIGN (0x1000): AT(ADDR(.data)-0xC0000000)
    {
        *(.data)
        *(.data.*)
        *(.got.plt)
    }
    
    .bss ALIGN (0x1000): AT(ADDR(.bss)-0xC0000000)
    {
        *(COMMON)
        *(.bss)
        *(.bss*)
    }

    .eh_frame ALIGN (0x1000): AT(ADDR(.eh_frame)-0xC0000000)
    {
        *(.eh_frame)
    }

    .comment ALIGN (0x1000): AT(ADDR(.comment)-0xC0000000)
    {
        *(.comment)
    }
}

KERNEL_PROGRAM_END = .;