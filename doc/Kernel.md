#### [Home](README.md)
## 4. Kernel

### 4.1 Memory
#### 4.1.1 Detecting Memory's Limits
- BIOS keeps a record of types of RAM.
- Thus BIOS can detect the type of RAM installed and its features.
- Because BIOS is in ROM, it can mess with RAM without consequences.
- So we ask BIOS for info on RAM.
#### 4.1.2 Global Descriptor Table
![GDT](resources/GDT.jpg)
#### 4.1.3 Physical Memory Map
![PhysMem](resources/Physical_Memory_Map.jpg)
#### 4.1.4 Reserved Memory Space On x86
![PhysMem](resources/Reserved_x86_Memory.png)
#### 4.1.5 Paging
![Paging](resources/Paging.jpg)
- With Paging enabled, when a memory address is deferenced, x86 goes to the Paging Directory
to look for the corresponding Page Table that manages 4MiB of the physical memory region.
- Once paging is enabled, everything must use paging, including the kernel (even interrupts).

```
// 2nd Page Table is looked up (because each Page Table manages 4MiB), 
// then 4 bytes at offset 0x100000 in the Page Table is set to 0x2.
int *p = 0x500000;
*p = 0x2;
```
#### 4.1.6 Higher-Half Kernel
- Because a user program always assumes it owns all the memory. All compiled programs starts 
at 0x0 in virtual memory, so we should place the kernel somewhere else so they don't overlap.
- With a lot of linker and paging-on-boot hacking, we place the kernel at 0xc0000000 (3GiB).
- From now on, all absolute physical addresses must add 0xc0000000 to its address (e.g. the VGA 
buffer must be at 0xc00b8000 instead of 0x000b8000). Everything that requires physical address 
to work must also be translated before passed in as arguments(e.g. the Page Directory in paging_init).
- All future user programs may easily make system calls as the kernel is in their address space.

```
higher_half_init() -> kboot() -> kmain() -> paging_init()
```

#### 4.1.7 Memory Management Unit
TODO

### 4.2 Interrupts
#### 4.2.1 Interrupt Descriptor Table
![IDT](resources/IDT.jpg)
#### 4.2.2 Programmable Interrupt Controller
![PIC](resources/PIC.jpg)
#### 4.2.3 Interrupt Handler Implementation
![Interrupts](resources/Interrupts.jpg)
#### 4.2.4 Interrupts
##### 4.2.4.1 Keyboard
- When a key is pressed, the keyboard sends an interrupt to the PIC, which is then sent to the CPU.
The interrupt number is 0x1, plus the PIC offset (0x20 in our case) becomes 0x21.
- When we receive this interrupt, the ISR must read the scan code from the keyboard port, then translate
this scan code into ascii code.
- Both key presses and key releases generate interrupts. If we hold down a key, interrupts will keep 
getting triggered.

PS/2:
```
Key press-> keyboard controller recognizes press -> sends interrupt to CPU
```
vs USB:
```
Key press -> keyboard controller recognizes press -> stores keypress until USB poll -> sends status to USB controller -> USB controller sends interrupt to CPU
```

### 4.3 Scheduler
