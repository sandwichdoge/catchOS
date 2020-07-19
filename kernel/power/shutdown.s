global shutdown
global enter_v86

enter_v86:
   mov ebp, esp               ; save stack pointer

   push dword  [ebp+4]        ; ss
   push dword  [ebp+8]        ; esp
   pushfd                     ; eflags
   or dword [esp], (1 << 17)  ; set VM flags
   push dword [ebp+12]        ; cs
   push dword  [ebp+16]       ; eip
   iret


shutdown:
   mov ax, 0x1000
   mov ax, ss
   mov sp, 0xf000
   mov ax, 0x5307
   mov bx, 0x0001
   mov cx, 0x0003
   int 0x15

   ret  ;if interrupt doesnt work
