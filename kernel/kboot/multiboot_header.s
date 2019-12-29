MAGIC_NUMBER    equ 0x1BADB002                      ; define the magic number constant
ALIGN_MODULES   equ 0x1
CHECKSUM        equ -(MAGIC_NUMBER + ALIGN_MODULES) ; calculate the checksum
                                                    ; (magic number + checksum + flags should equal 0)

section .multiboot
align 4                         ; the code must be 4 byte aligned
    dd MAGIC_NUMBER             ; write the magic number to the machine code,
    dd ALIGN_MODULES
    dd CHECKSUM                 ; and the checksum
