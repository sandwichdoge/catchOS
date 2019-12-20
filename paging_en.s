global loadPageDirectory
global enablePaging

loadPageDirectory:
    mov eax, [esp + 4]
    mov cr3, eax
    ret

enablePaging:
    mov eax, cr0
    or eax, 0x80000000 ; set 32th bit of cr0
    mov cr0, eax
    ret