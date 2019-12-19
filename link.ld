ENTRY(loader)

SECTIONS {
    . = 0x00200000;

    .rodata ALIGN (0x0000):
    {
        *(.multiboot)
        *(.rodata)
    }

    .text ALIGN (0x1000):
    {
        *(.text)
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