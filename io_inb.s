global inb

inb:
    mov dx, [esp + 4] ; [esp + 4] is address of I/O port
    in al, dx         ; Read 1 byte from I/O port to al
    ret               ; Return the read byte