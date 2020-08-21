global gdt_descriptor

align 4096
gdt_start:
; with this setup we're telling the cpu that kernel code & data segments can be anywhere in 4GB memory
gdt_null:        ; null gdt entry, mandatory
    dd 0x0
    dd 0x0

gdt_code:        ; kernel code section
    dw 0xffff    ; limit (bits 0-15)
    dw 0x0       ; base (bits 0-15)
    db 0x0       ; base (bits 16-23)
    db 10011010b ; access byte
    db 11001111b ; flags + limit (bits 16-19)
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

; Offsets of GDT entries in GDT, unused for now
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
