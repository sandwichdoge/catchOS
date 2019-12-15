; kboot is the entry of the kernel.
; it does a few things: load gdt into cpu, init protected mode
; then start kmain, which now runs in 32-bit protected mode.

global switch_to_pm
global kboot
extern load_gdt
extern kmain

switch_to_pm:              ; switch from 16-bit real mode to 32-bit protected mode
    cli                    ; must disable interrupts
    ; Next 3 instructions switch us to 32-bit protected mode
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp 0x08:init_pm       ; put 0x08 in cs (which is an offset in the GDT for kernel code segment)

[bits 32]                  ; tell assembler to encode in 32-bit mode instructions
init_pm:
    mov ax, 0x10           ; 0x10 is the offset of data segment in GDT
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x1fffff      ; kernel stack base
    mov esp, ebp
    ; call something here, never return from init_pm
    call kmain
    jmp $

kboot:
    call load_gdt
    call init_pm