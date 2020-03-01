global load_page_directory
global enable_paging
global disable_paging

load_page_directory:
    mov eax, [esp + 4]
    mov cr3, eax
    ret

enable_paging:
    mov eax, cr0
    or eax, 0x80000000 ; set 32th bit of cr0
    mov cr0, eax
    ret

disable_paging:
    mov eax, cr0
    and eax, (1<<31)-1
    mov cr0, eax
    ret