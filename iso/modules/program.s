bits 32 ; 32 bit instructions otherwise all 4 bytes will not get moved to eax
mov eax, 2 ; sbrk
int 128
ret