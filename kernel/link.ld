ENTRY(loader)

SECTIONS {
    . = 0x01000;
    KERNEL_START_PHYS = . - 0x0;
    KERNEL_START_VIRTUAL = .;

    .multiboot ALIGN (0x8): AT(ADDR(.multiboot)-0x0)
    {
        *(.multiboot)
    }

    .rodata ALIGN (0x1000): AT(ADDR(.rodata)-0x0)
    {
        *(.rodata*)
    }

    .SMPBOOT_TRAMPOLINE_FUNC ALIGN (0x1000): AT(ADDR(.SMPBOOT_TRAMPOLINE_FUNC)-0x0)
    {
        *(.SMPBOOT_TRAMPOLINE_FUNC)
        *(.SMPBOOT_TRAMPOLINE_*)
    }

    .text ALIGN (0x1000): AT(ADDR(.text)-0x0)
    {
        *(.text)
        *(.text*)
    }

    .data ALIGN (0x1000): AT(ADDR(.data)-0x0)
    {
        *(.data)
        *(.data.*)
    }
    
    .bss ALIGN (0x1000): AT(ADDR(.bss)-0x0)
    {
        *(COMMON)
        *(.bss)
    }

    .comment ALIGN (0x1000): AT(ADDR(.comment)-0x0)
    {
        *(.comment)
    }

    KERNEL_END_VIRTUAL = .;
    KERNEL_END_PHYS = . - 0x0;
}
