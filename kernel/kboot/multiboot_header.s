MAGIC_NUMBER    equ 0x1BADB002                      ; define the magic number constant
MB_FLAGS        equ 0x5
CHECKSUM        equ -(MAGIC_NUMBER + MB_FLAGS) ; calculate the checksum
                                                    ; (magic number + checksum + MB_FLAGS should equal 0)

section .multiboot
align 4                         ; the code must be 4 byte aligned
    dd MAGIC_NUMBER             ; write the magic number to the machine code,
    dd MB_FLAGS
    dd CHECKSUM                 ; and the checksum
