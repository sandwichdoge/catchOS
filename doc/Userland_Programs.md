## 5. Userland Programs
- We will use elf files for our user programs.
- Kernel loads user program ELF from a file, parse .text, .rodata, .data , .bss sections and load them 
into an executable memory image. This means we can't run C++ programs because they have extra sections.
