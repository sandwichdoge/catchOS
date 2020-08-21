global SMPBOOT_TRAMPOLINE_FUNC
global SMPBOOT_TRAMPOLINE_PARAMS
extern gdt_descriptor
extern AP_STARTUP_SUCCESSFUL    ; Modify this flag on successful startup to let BSP know we're good

; All APs go through the same trampoline code one at a time. We do not start all APs at the same time in parallel here.

section .SMPBOOT_TRAMPOLINE_PARAMS
SMPBOOT_TRAMPOLINE_PARAMS:
TRAMPOLINE_STACK:
    dd 1
TRAMPOLINE_IDT:
    dd 1
TRAMPOLINE_KERNEL_PD:
    dd 1

[BITS 16]
section .SMPBOOT_TRAMPOLINE_FUNC
SMPBOOT_TRAMPOLINE_FUNC:
    cli
    lgdt [gdt_descriptor]
    ; Next 3 instructions switch us to 32-bit protected mode
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    jmp 0x08:prot_mode

[BITS 32]
prot_mode:

    mov ax, 0x10           ; 0x10 is the offset of data segment in GDT
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, [TRAMPOLINE_STACK]
    mov esp, eax
    mov ebx, [TRAMPOLINE_KERNEL_PD]
    mov cr3, ebx
    mov ecx, [TRAMPOLINE_IDT]
    lidt [ecx]
    mov eax, 1
    mov [AP_STARTUP_SUCCESSFUL], eax    ; Let BSP know we have successfully started.
    sti
    hlt
    hlt
    hlt
    hlt
