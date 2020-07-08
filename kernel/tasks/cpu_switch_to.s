global cpu_switch_to

;https://wiki.osdev.org/Brendan%27s_Multi-tasking_Tutorial
cpu_switch_to:
;[esp + 4] = prev TCB
;[esp + 8] = next TCB

    ;Save previous task's state
    ;Notes:
    ;  For cdecl; EAX, ECX, and EDX are already saved by the caller and don't need to be saved again
    ;  EIP is already saved on the stack by the caller's "CALL" instruction
    ;  The task isn't able to change CR3 so it doesn't need to be saved
    ;  Segment registers are constants (while running kernel code) so they don't need to be saved

    push ebx
    push esi
    push edi
    push ebp

    mov edi, [esp + 16 + 4]         ;edi = address of the previous task's "thread control block"
    mov [edi + 16], esp             ;Save ESP for previous task's kernel stack in the thread's TCB

    xchg bx, bx
    ;Load next task's state
 
    lea esi, [esp + 16 + 8]         ;esi = address of the next task's "thread control block" (parameter passed on stack)
    mov esp, [esi + 16]             ;Load ESP for next task's kernel stack from the thread's TCB
    ;mov eax,[esi+TCB.CR3]          ;eax = address of page directory for next task
    ;mov ebx,[esi+TCB.ESP0]         ;ebx = address for the top of the next task's kernel stack
    ;mov [TSS.ESP0],ebx             ;Adjust the ESP0 field in the TSS (used by CPU for for CPL=3 -> CPL=0 privilege level changes)
    ;mov ecx,cr3                    ;ecx = previous task's virtual address space

    ;cmp eax,ecx                     ;Does the virtual address space need to being changed?
    ;je .doneVAS                     ; no, virtual address space is the same, so don't reload it and cause TLB flushes
    ;mov cr3,eax                     ; yes, load the next task's virtual address space
.doneVAS:
 
    pop ebp
    pop edi
    pop esi
    pop ebx
 
    ret                             ;Load next task's EIP from its kernel stack. https://docs.oracle.com/cd/E19455-01/806-3773/instructionset-67/index.html
                                    ; since ret instruction will jump to the address at top of stack, which was pushed by call instruction.