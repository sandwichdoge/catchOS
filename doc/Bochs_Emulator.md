#### [Home](README.md)
# 2.Bochs Emulator

## 2.1. Introduction
Normal VMs like VMWare and VirtualBox insert Guest instructions in favor of performance so it's 
not accurate, so we use a CPU emulator like qemu or bochs. We pick Bochs for being lightweight, 
verbose logging and auto pause on reset trap.

How to install with apt:
```
sudo apt install build-essential nasm genisoimage bochs bochs-sdl libsdl2-2.0
```

## 2.2. Configuration
A typical bochsrc.txt config file looks like this:
```
megs: 32
display_library: sdl2
romimage: file=/usr/share/bochs/BIOS-bochs-latest
vgaromimage: file=/usr/share/bochs/VGABIOS-lgpl-latest
ata0: enabled=1, ioaddr1=0x1f0, ioaddr2=0x3f0, irq=14
ata1: enabled=1, ioaddr1=0x170, ioaddr2=0x370, irq=15
ata0-master: type=cdrom, path=os.iso, status=inserted
ata1-master: type=disk, mode=flat, cylinders=306, heads=4, spt=17, translation=none
boot: cdrom
log: bochslog.txt
clock: sync=realtime, time0=local
cpu: count=1, ips=1000000
com1: enabled=1, mode=file, dev=com1.out
magic_break: enabled=1
```

## 2.3. Running
```
bochs -f bochsrc.txt -q
```

## 2.4. Debugging
Write this into your bochsrc: "magic_break: enabled=1", and put "xchg bx,bx" in your code 
just before the faulting instruction. When bochs starts, debugger prompt will appear (bochs:1>), 
type "c" and Enter here. After that, the debugger will be called the 2nd time, enter:

```
    reg
    creg
    info gdt
    info idt
    info tab     // Paging table
    x /16 [addr] // Print 16 values at addr in memory, 4 bytes in size each
```

http://bochs.sourceforge.net/doc/docbook/user/internal-debugger.html

https://thestarman.pcministry.com/linux/bochsrc.bxrc.htm

