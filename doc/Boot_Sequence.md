#### [Home](README.md)
## 3. Boot Sequence
![Boot Sequence](resources/Boot_Sequence.jpg)

Our kernel is loaded at 0x00200000 in physical memory and 0xc0200000 (3GiB) in virtual memory (Higher-Half Kernel).
To do this, the first thing kboot does is setup a page table to map the first 4 MiB of physical memory to 0xc0000000 of virtual memory.
After that, kmain will properly setup and manage the virtual memory space of the kernel and its child processes.

Please see link.ld for the entry function.
