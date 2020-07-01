extern interrupt_handler 

; NASM template macro, from Little OS Book
%macro no_error_code_interrupt_handler 1
global asm_int_handler_%1
asm_int_handler_%1:
    push	dword 0                     ; push 0 as error code
    push	dword %1                    ; push the interrupt number
    jmp	    asm_int_handler_common      ; jump to the common handler
%endmacro

%macro error_code_interrupt_handler 1
global asm_int_handler_%1
asm_int_handler_%1:
    push    dword %1                    ; push the interrupt number
    jmp     asm_int_handler_common      ; jump to the common handler
%endmacro

no_error_code_interrupt_handler 32      ; create handler for interrupt 0 (system timer)
no_error_code_interrupt_handler	33	    ; create handler for interrupt 1 (keyboard)
no_error_code_interrupt_handler 14	    ; create handler for interrupt 2 (paging)
no_error_code_interrupt_handler 128     ; create handler for interrupt 2 (syscall)


asm_int_handler_common:
    push eax
    push ebx
    push ecx
    push edx
    push ebp
    push esi
    push edi
    lea eax, [esp+24]   ; modifying value in this address will change (1), return reg
    push eax
    call interrupt_handler
    pop eax
    pop	edi
    pop	esi
    pop	ebp
    pop	edx
    pop	ecx
    pop	ebx
    pop eax             ; (1) this
    add esp, 8  ; restore esp from int_handler_33 arg push
    iret        ; return from interrupt

