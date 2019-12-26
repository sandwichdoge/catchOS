global loader                   ; the entry symbol for ELF
extern kboot                    ; kboot will switch to protected mode and call kmain

KERNEL_STACK_SIZE equ 4096      ; size of stack in bytes

section .text                   ; start of the text (code) section
loader:                         ; the loader label (defined as entry point in linker script)
    mov esp, kernel_stack + KERNEL_STACK_SIZE	; point esp to the start of the
                                                ; stack (end of memory area)

higher_half_init:               ; initialize higher-half kernel
    ; init first page table here
    xor ebx, ebx
    lea edi, [asm_first_page_table - 0xc0000000]
.fill_table:
    mov eax, ebx
    mov edx, 0x1000
    mul edx             ; multiply index with 0x1000
    mov ecx, eax        ; put multiplied value in ecx
    or ecx, 3
    ; asm_first_page_table + (ebx * 4) = ecx
    mov [edi + ebx * 4], ecx
    inc ebx
    cmp ebx, 1024
    jne .fill_table

    ; finished filling table for first 4MiB, now put it in page directory
    lea esi, [asm_first_page_table - 0xc0000000] ; page Table is now in esi
    or esi, 3
    
    ; asm_page_directory[0] = esi; asm_page_directory[768] = esi;
    mov [asm_page_directory - 0xc0000000], esi
    mov [asm_page_directory - 0xc0000000 + 768 * 4], esi
    lea esi, [asm_page_directory - 0xc0000000]

    ; load page directory
    mov cr3, esi
    
    ; enable paging
    mov eax, cr0
    or eax, 0x80000000  ; set 32th bit of cr0
    mov cr0, eax

    jmp kboot


section .bss                    ; our stack is in uninitialized data section
align 4096				                  ; align at 4 bytes
kernel_stack:                   ; label points to beginning of memory
    resb KERNEL_STACK_SIZE      ; reserve stack for the kernel
asm_page_directory: ; ERROR: Need to align our directory here?
    resb 4096
asm_first_page_table:
    resb 4096
