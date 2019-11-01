global outb

outb:
    push ebp
    mov ebp, esp
    mov al, [ebp + 8] ; Data byte
    mov dx, [ebp + 4] ; I/O port
    out dx, al        ; out instruction
    pop ebp
    ret