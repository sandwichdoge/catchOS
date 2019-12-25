global loadPageDirectory
global enablePaging
global disablePaging

loadPageDirectory:
    mov eax, [esp + 4]
    mov cr3, eax
    ret

enablePaging:
    mov eax, cr0
    or eax, 0x80000000 ; set 32th bit of cr0
    mov cr0, eax
    ret

disablePaging:
    mov eax, cr0
    and eax, (1<<31)-1
    mov cr0, eax
    ret