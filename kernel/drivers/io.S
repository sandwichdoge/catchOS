global inb
global inw
global outb
global outw
global io_wait

inb:
    mov dx, [esp + 4] ; [esp + 4] is address of I/O port
    in al, dx         ; Read 1 byte from I/O port to al
    ret               ; Return the read byte

inw:
    mov dx, [esp + 4]
    in ax, dx
    ret

outb:
    mov al, [esp + 8]    ; move the data to be sent into the al register
    mov dx, [esp + 4]    ; move the address of the I/O port into the dx register
    out dx, al           ; send the data to the I/O port
    ret                  ; return to the calling function

outw:
    mov ax, [esp + 8]
    mov dx, [esp + 4]
    out dx, ax
    ret

io_wait:
    jmp 1fh
a1:
    jmp 1fh
b1: