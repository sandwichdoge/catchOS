global asm_int_handler_33    ; keyboard
global asm_int_handler_14    ; pagefault
extern interrupt_handler 

asm_int_handler_14:
    push 0
    push dword 14
    jmp asm_int_handler_common

asm_int_handler_33:
    push 0
    push dword 33
    jmp asm_int_handler_common


asm_int_handler_common:
    push eax
    push ebx
    push ecx
    push edx
    push ebp
    push esi
    push edi
    call interrupt_handler
    pop	edi
    pop	esi
    pop	ebp
    pop	edx
    pop	ecx
    pop	ebx
    pop eax
    add esp, 8  ; restore esp from int_handler_33 arg push
    iret        ; return from interrupt

