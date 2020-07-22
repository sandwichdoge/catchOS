section .multiboot
header_start:
    dd 0xe85250d6                ; magic number (multiboot 2)
    dd 0                         ; architecture 0 (protected mode i386)
    dd header_end - header_start ; header length
    ; checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ; insert optional multiboot tags here

framebuffer_header_start: align 8
    dw 5
    dw 1
    dd framebuffer_header_end - framebuffer_header_start
    dd 640
    dd 480
    dd 32
framebuffer_header_end: align 8

    ; required end tag
    dw 0    ; type
    dw 0x0    ; flags
    dd 8    ; size
header_end:
