global get_phys_mem_size

get_phys_mem_size:
    mov di, esp       ; This is ok because now all segment register are 0x10 (except cs which is 0x8)
    xor ebx, ebx
    mov edx, 0x534D4150
    mov eax, 0xe820
    mov ecx, 24
    int 0x15
