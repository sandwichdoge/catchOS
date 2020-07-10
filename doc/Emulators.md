#### [Home](README.md)

- [2.Bochs Emulator](#2bochs-emulator)
* [2.1. Introduction](#21-introduction)
* [2.2. Configuration](#22-configuration)
* [2.3. Running](#23-running)
* [2.4. Debugging](#24-debugging)
* [2.5. Logging](#25-logging)

# 2.1. Bochs Emulator

## 2.1.1. Introduction
Normal VMs like VMWare and VirtualBox insert Guest instructions in favor of performance so it's 
not accurate, so we use a CPU emulator like qemu or bochs. We pick Bochs for being lightweight, 
verbose logging and auto pause on reset trap.

How to install with apt:
```
sudo apt install build-essential nasm genisoimage bochs bochs-sdl libsdl2-2.0
```

## 2.1.2. Configuration
A typical bochsrc.txt config file looks like this:
```
megs: 32
display_library: sdl2
romimage: file=/usr/share/bochs/BIOS-bochs-latest
vgaromimage: file=/usr/share/bochs/VGABIOS-lgpl-latest
ata0: enabled=1, ioaddr1=0x1f0, ioaddr2=0x3f0, irq=14
ata1: enabled=1, ioaddr1=0x170, ioaddr2=0x370, irq=15
ata0-master: type=disk, path="hdd.img", mode=flat, cylinders=20, heads=16, spt=63
ata1-slave: type=cdrom, path="os.iso", status=inserted
boot: cdrom
log: bochslog.txt
clock: sync=realtime, time0=local
cpu: count=1, ips=1000000
com1: enabled=1, mode=file, dev=serial_debug.txt
magic_break: enabled=1
```

## 2.1.3. Running
```
bochs -f bochsrc.txt -q
```
When started, bochs may pause the emulation. Switch to bochs console, type 'c', then press Enter to continue.


## 2.1.4. Debugging
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

We also use _dbg_serial() to print out debug messages. Serial log file is saved to ```output/serial_debug.txt```.

http://bochs.sourceforge.net/doc/docbook/user/bochsrc.html

http://bochs.sourceforge.net/doc/docbook/user/internal-debugger.html

https://thestarman.pcministry.com/linux/bochsrc.bxrc.htm


## 2.1.5. Logging
As configured in bochsrc.txt file, log will be written to output/bochslog.txt.

# 2.2. QEMU
## 2.2.1. Introduction
Boots a little faster than Bochs, but not as convenient to debug.

## 2.2.2 Running
```
qemu-system-i386 -accel tcg,thread=single -cpu core2duo -m 128 -no-reboot -drive format=raw,media=cdrom,file=os.iso -serial file:serial_debug.txt -smp 1 -usb -vga std -s
```

## 2.2.3. Configurations
Serial console redirection:
```
-serial file:serial_debug.txt
-serial stdio
```

Enable gdb debugging:
```
-s
```

Make CPU not start immediately on startup:
```
-S
```

## 2.2.4. Debugging
- QEMU must be run with gdb debugging option:
```
-s
```
On host machine, run commands:
```
gdb
target remote localhost:1234
```

- How to add breakpoints:

```
// In code, add this line. This is basically an endless loop.
asm volatile ("1: jmp 1b");
```

```
// In gdb, at program pause, enter command to jump out of endless loop:
(gdb) set $pc += 2
```
