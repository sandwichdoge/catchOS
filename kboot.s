global load_gdt
global switch_to_pm
extern write_cstr
extern kmain

gdt_start:

gdt_null:
    dd 0x0
    dd 0x0

gdt_code:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0

gdt_data:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b
    db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; Offsets in GDT
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start


load_gdt:                    ; load global descriptor table into processor
    lgdt [gdt_descriptor]
    ret

switch_to_pm:           ; switch from 16-bit real mode to 32-bit protected mode
    cli
    ; Next 3 instructions switch us to 32-bit protected mode
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:init_pm   ; put 0x08 in cs (which is an offset in the GDT for kernel code segment)


[bits 32]
init_pm:
    ; ISSUE: Bochs hangs up when we try to modify segment registers
    mov ax, DATA_SEG
    ;mov ds, ax
    ;mov ss, ax
    ;mov es, ax
    ;mov fs, ax
    ;mov gs, ax

    mov ebp, 0x90000
    mov esp, ebp
    ; call something here, never return from init_pm
    call kmain
    jmp $

MSG_PROT_MODE: 
    db "ProtMode"