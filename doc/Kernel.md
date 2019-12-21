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
#### 4.1.5 Memory Managerment Unit
TODO

### 4.2 Interrupts
#### 4.2.1 Interrupt Descriptor Table
![IDT](resources/IDT.jpg)
#### 4.2.2 Programmable Interrupt Controller
![PIC](resources/PIC.jpg)
#### 4.2.3 Interrupt Handler Implementation
![Interrupts](resources/Interrupts.jpg)

### 4.3 Paging

### 4.4 Scheduler
