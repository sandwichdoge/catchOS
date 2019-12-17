ENTRY(loader)

SECTIONS {
    . = 0x00200000;

    .text ALIGN (0x1000):
    {
        *(.text)
        *(.rodata)
    }

    .data ALIGN (0x1000):
    {
        *(.data)
    }
    
    .bss ALIGN (0x1000):
    {
        *(COMMON)
        *(.bss)
    }
}

KERNEL_PROGRAM_END = .;