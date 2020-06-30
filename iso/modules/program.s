bits 32 ; 32 bit instructions otherwise all 4 bytes will not get moved to eax
mov eax, 2  ; mmap
mov ecx, 64 ; bytes to request
int 128
xchg bx, bx ; expect 0xc0008000
mov ecx, eax; allocated mem
mov eax, 3  ; munmap
int 128
xchg bx, bx ;
ret