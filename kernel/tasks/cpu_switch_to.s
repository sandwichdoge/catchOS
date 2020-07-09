global cpu_switch_to

;https://wiki.osdev.org/Brendan%27s_Multi-tasking_Tutorial
cpu_switch_to:
;[esp + 4] = prev TCB
;[esp + 8] = next TCB
    ;Save previous task's state
    ;Notes:
    ;  EIP is already saved on the stack by the caller's "CALL" instruction
    ;  The task isn't able to change CR3 so it doesn't need to be saved
    ;  Segment registers are constants (while running kernel code) so they don't need to be saved
    push eax
    push ecx
    push edx
    push ebx
    push esi
    push edi
    push ebp

    mov edi, [esp + 28 + 4]         ;edi = address of the previous task's "thread control block"
    test edi, edi
    jz .load

    mov [edi + 16], esp             ;Save ESP for previous task's kernel stack in the thread's TCB

.load:

    ;Load next task's state
    mov esi, [esp + 28 + 8]         ;esi = address of the next task's "thread control block" (parameter passed on stack)
    mov esp, [esi + 16]             ;Load ESP for next task's kernel stack from the thread's TCB
    ;mov eax,[esi+TCB.CR3]          ;eax = address of page directory for next task
    ;mov ebx,[esi+TCB.ESP0]         ;ebx = address for the top of the next task's kernel stack
    ;mov [TSS.ESP0],ebx             ;Adjust the ESP0 field in the TSS (used by CPU for for CPL=3 -> CPL=0 privilege level changes)
    ;mov ecx,cr3                    ;ecx = previous task's virtual address space

    ;cmp eax,ecx                     ;Does the virtual address space need to being changed?
    ;je .doneVAS                     ; no, virtual address space is the same, so don't reload it and cause TLB flushes
    ;mov cr3,eax                     ; yes, load the next task's virtual address space
    test edi, edi
    jz .first
.doneVAS:
    pop ebp
    pop edi
    pop esi
    pop ebx
    pop edx
    pop ecx
    pop eax

    ret                             ;Load next task's EIP from its kernel stack. https://docs.oracle.com/cd/E19455-01/806-3773/instructionset-67/index.html
                                    ; since ret instruction will jump to the address at top of stack, which was pushed by call instruction.
.first:
    mov esi, [esi + 52]             ;Offset of eip in TCB
    push esi
    ret
