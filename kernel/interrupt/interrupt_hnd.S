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

no_error_code_interrupt_handler 32      ; hw interrupt 0 (system timer)
no_error_code_interrupt_handler	33	    ; hw interrupt 1 (keyboard)
no_error_code_interrupt_handler 36      ; hw interrupt 4 (com1)
no_error_code_interrupt_handler 39      ; hw device_not_available
no_error_code_interrupt_handler 15	        ; exception interrupt 14 (paging)
error_code_interrupt_handler 14	        ; exception interrupt 14 (paging)
error_code_interrupt_handler 13	        ; exception interrupt 13 (general protection fault)
no_error_code_interrupt_handler 128     ; sw interrupt 128 (syscall)


asm_int_handler_common:
    push eax
    push ebx
    push ecx
    push edx
    push ebp
    push esi
    push edi
    push esp
    lea eax, [esp+28]   ; modifying value in this address will change (1), return reg
    push eax
    call interrupt_handler
    pop eax
    pop esp
    pop	edi
    pop	esi
    pop	ebp
    pop	edx
    pop	ecx
    pop	ebx
    pop eax             ; (1) this
    add esp, 8  ; restore esp from int_handler_33 arg push
    iret        ; return from interrupt - iret pops stack_state flags that was automatically added when interrupt happened

;https://docs.oracle.com/cd/E19620-01/805-4693/instructionset-75/index.html
