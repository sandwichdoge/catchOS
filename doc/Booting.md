#### [Home](README.md)

- [3. Booting](#3-booting)
* [3.1. Boot Sequence](#31-boot-sequence)
* [3.2. Setup for High-Half Kernel](#32-setup-for-high-half-kernel)
* [3.3. Multiboot Specification](#33-multiboot-specification)
* [3.4. Kernel Entry](#34-kernel-entry)

## 3. Booting
### 3.1. Boot Sequence
![Boot Sequence](resources/Boot_Sequence.jpg)

### 3.2. Setup for High-Half Kernel
Our kernel is loaded at 0x00200000 in physical memory and 0xc0200000 (3GiB) in virtual memory (Higher-Half Kernel).
To do this, the first thing kboot does is setup a page table to map the first 4 MiB of physical memory to 0xc0000000 of virtual memory.
After that, kmain will properly setup and manage the virtual memory space of the kernel and its child processes.

### 3.3. Multiboot Specification
Multiboot spec is an x86 standard. An OS image must contain a Multiboot Header within the first 8192 bytes of the file and must be aligned on 4 bytes.

A Multiboot compliant bootloader must be able to load ELF binaries, such as K.

In the Multiboot Header, you should have the following fields:
- magic, a u32 used to identify the header (value: 0x1BADB002)
- flags, a u32 field used to specify the needed features. You should need the flag asking the bootloader to align all modules on a page boundary (0x1) 
and the flag asking the bootloader to pass memory informations to the kernel (0x2).
- checksum, a u32 field whose value is -(magic + flags)

The state of the system when the kernel starts to be executed is the following:
- EAX contains a magic value 0x2BADB002
- EBX contains the address of the Multiboot information structure
- The system is in 32 bits

The bootloader fills a structure called Multiboot infortmation structure. It is described in multiboot.h by multiboot_info_t.

You will find in this structure the fields mods_count and mods_addr. mods_addr contains the address of an array of informations about the loaded modules, such as the address where it is loaded and the size of the module. This structure is defined in multiboot.h as module_t. The modules to load are specified in the grub.cfg. For K, the first module is the rom you should load. 
```We load user programs as grub modules in early stages because no hard disk driver yet```.

You will need the command line passed to your kernel. You can find its address in the cmdline field of your Multiboot information structure.

https://www.gnu.org/software/grub/manual/multiboot/multiboot.html

### 3.4. Kernel Entry
Please see ```link.ld``` for the entry function.
