ENTRY(loader)
phys = 0x00200000;
SECTIONS
{
.text phys : AT(phys) {
    code = .;
    *(.booter)
    *(.text)
    *(.rodata)
    . = ALIGN(0);
}
.data : AT(phys + (data - code))
{
    data = .;
    *(.data)
    . = ALIGN(4096);
}
.bss : AT(phys + (bss - code))
{
    bss = .;
    *(.bss)
    . = ALIGN(4096);
}
end = .;
}
KERNEL_PROGRAM_END = .;