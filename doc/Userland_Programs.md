## 5. Userland Programs
- We will use elf files for our user programs.
- Kernel loads user program ELF from a file, parse .text, .rodata, .data , .bss sections and load them 
into an executable memory image. This means we can't run C++ programs because they have extra sections.

### 5.1 User Program's Layout
- The memory image (.text section) must be copied to the pages used for program code.
- We allocate one page for user stack.
- User stack starts at 0xBFFFFFFF (just below Kernel Program) and grows downwards.
- Data and code will be mapped at 0x0.
- So at least 2 pages are needed.
- We will store these info in a struct representing a process. This struct can be dynamically allocated 
with the kernel's malloc function.
