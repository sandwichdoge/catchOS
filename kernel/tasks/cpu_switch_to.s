global cpu_switch_to

STRUCT_TASK_ESP equ 0
STRUCT_TASK_EIP equ 36
PUSHED_REGS_SZ equ 32   ; How many bytes ESP has move after storing all 8 cpu regs

;https://wiki.osdev.org/Brendan%27s_Multi-tasking_Tutorial
cpu_switch_to:
    ;[esp + 4] = prev TCB
    ;[esp + 8] = next TCB
    ;Save previous task's state
    ;Notes:
    ;  The task isn't able to change CR3 so it doesn't need to be saved
    ;  Segment registers are constants (while running kernel code) so they don't need to be saved
    push eax
    push ecx
    push edx
    push ebx
    push esi
    push edi
    push ebp
    pushf

    mov edi, [esp + PUSHED_REGS_SZ + 4] ;edi = address of the previous task's "struct_task/TCB"

    mov eax, [esp + PUSHED_REGS_SZ]     ;EAX is this func's return address i.e. where the prev task left off before calling cpu_switch_to()
    mov [edi + STRUCT_TASK_EIP], eax    ;save EIP for previous task's eip
    mov [edi + STRUCT_TASK_ESP], esp    ;save ESP for previous task's kernel stack in the thread's TCB
.load:

    ;Load next task's state
    mov esi, [esp + PUSHED_REGS_SZ + 8] ;esi = address of the next task's "struct_task/TCB" (parameter passed on stack)
    mov esp, [esi + STRUCT_TASK_ESP]    ;Load ESP for next task's kernel stack from the thread's TCB

    ;mov eax,[esi+TCB.CR3]              ;eax = address of page directory for next task
    ;mov ebx,[esi+TCB.ESP0]             ;ebx = address for the top of the next task's kernel stack
    ;mov [TSS.ESP0],ebx                 ;Adjust the ESP0 field in the TSS (used by CPU for for CPL=3 -> CPL=0 privilege level changes)
    ;mov ecx,cr3                        ;ecx = previous task's virtual address space

    ;cmp eax,ecx                        ;Does the virtual address space need to being changed?
    ;je .doneVAS                        ; no, virtual address space is the same, so don't reload it and cause TLB flushes
    ;mov cr3,eax                        ; yes, load the next task's virtual address space

    ; Modify return address of self. Put in next task's eip.
    mov eax, [esi + STRUCT_TASK_EIP]
    mov [esp + PUSHED_REGS_SZ], eax

.doneVAS:
    popf
    pop ebp
    pop edi
    pop esi
    pop ebx
    pop edx
    pop ecx
    pop eax

    ret             ;Load next task's EIP. https://docs.oracle.com/cd/E19455-01/806-3773/instructionset-67/index.html
                    ; since ret instruction will jump to the address at top of stack, which was pushed by call instruction.
