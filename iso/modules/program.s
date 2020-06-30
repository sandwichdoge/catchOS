bits 32 ; 32 bit instructions otherwise all 4 bytes will not get moved to eax
mov eax, 2  ; sbrk
mov ecx, 40 ; bytes to request
int 128
;xchg bx, bx ; expect 0xaaaabbbb
ret