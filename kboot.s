global load_gdt
global switch_to_pm
global kboot
extern kmain

gdt_start:
; with this setup we're telling the cpu that kernel code & data segments can be anywhere in 4GB memory
gdt_null:        ; null gdt entry, mandatory
    dd 0x0
    dd 0x0

gdt_code:        ; kernel code section
    dw 0xffff    ; limit (bits 0-15)
    dw 0x0       ; base (bits 0-15)
    db 0x0       ; base (bits 16-23)
    db 10011010b ; type flag
    db 11001111b ; limit flags (bits 16-19)
    db 0x0       ; base (bit 24- 31)

gdt_data:         ; kernel data section
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0

gdt_end:


gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; size of GDT, must be true size - 1
    dd gdt_start               ; address of the GDT

; Offsets of GDT entries in GDT
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start


load_gdt:                    ; load global descriptor table into processor
    lgdt [gdt_descriptor]
    ret


switch_to_pm:              ; switch from 16-bit real mode to 32-bit protected mode
    cli                    ; must disable interrupts
    ; Next 3 instructions switch us to 32-bit protected mode
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:init_pm   ; put 0x08 in cs (which is an offset in the GDT for kernel code segment)

[bits 32]                  ; tell assembler to encode in 16-bit mode instructions
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000  ;
    mov esp, ebp
    ; call something here, never return from init_pm
    call kmain
    jmp $

kboot:
    call load_gdt
    call init_pm