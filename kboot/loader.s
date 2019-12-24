global loader                   ; the entry symbol for ELF
extern kboot                    ; kboot will switch to protected mode and call kmain

MAGIC_NUMBER equ 0x1BADB002     ; define the magic number constant
FLAGS        equ 0x0            ; multiboot flags
CHECKSUM     equ -MAGIC_NUMBER  ; calculate the checksum
                                ; (magic number + checksum + flags should equal 0)
KERNEL_STACK_SIZE equ 4096      ; size of stack in bytes

section .multiboot:
align 4                         ; the code must be 4 byte aligned
    dd MAGIC_NUMBER             ; write the magic number to the machine code,
    dd FLAGS                    ; the flags,
    dd CHECKSUM                 ; and the checksum

section .text:                  ; start of the text (code) section
loader:                         ; the loader label (defined as entry point in linker script)
    mov eax, 0xCAFEBABE         ; place the number 0xCAFEBABE in the register eax
    mov esp, kernel_stack + KERNEL_STACK_SIZE	; point esp to the start of the
                                                ; stack (end of memory area)
                                ; esp at 0xc020000 + KERNEL_STACK_SIZE + end_of_kernel
    jmp higher_half_init
    
.loop:
    jmp .loop                   ; loop forever

higher_half_init:
    ; init first page table here
    xor ebx, ebx
.loop2:
    mov eax, ebx
    mov edx, 0x1000
    mul edx             ; multiply index with 0x1000
    add eax, 0xc0000000 ; add virtual address starting
    mov ecx, eax        ; put multiplied value in ecx
    or ecx, 3
    ; asm_first_page_table + (ebx * 4) = ecx
    lea edx, [asm_first_page_table - 0xc0000000]
    mov [edx + ebx * 4], ecx
    inc ebx
    cmp ebx, 1024
    jne .loop2
hhboot:
    lea esi, [asm_first_page_table - 0xc0000000]
    xchg bx, bx ;chk esi, save this address, this address points to first table
    ; Page Table is now in esi
    or esi, 3
    mov [asm_page_directory - 0xc0000000], esi
    lea esi, [asm_page_directory - 0xc0000000]
    xchg bx, bx ;deref value in esi, should be first_page_table | 3
    mov cr3, esi        ; load page directory
    mov eax, cr0
    or eax, 0x80000000  ; set 32th bit of cr0
    mov cr0, eax
    xchg bx, bx
    jmp kboot


section .bss:                   ; our stack is in uninitialized data section
align 4				                  ; align at 4 bytes
kernel_stack:                   ; label points to beginning of memory
    resb KERNEL_STACK_SIZE      ; reserve stack for the kernel
asm_page_directory:
    TIMES 1024 dd 0
asm_first_page_table:
    TIMES 1024 dd 0
