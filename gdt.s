global lgdt
global lsr

lgdt:                    ; load global descriptor table into processor
    mov eax, [esp + 4]
    lgdt [eax]
    ret

lsr:
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    jmp 0x08:flush_cs   ; put 0x08 in cs (which is an offset in the GDT for kernel code segment)
flush_cs:
    ret