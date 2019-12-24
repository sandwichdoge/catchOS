#### [Home](README.md)
# 2.Bochs Emulator

Write this into your bochsrc: "magic_break: enabled=1", and put "xchg bx,bx" in your code 
just before the faulting instruction. When bochs starts, debugger prompt will appear (bochs:1>), 
type "c" and Enter here. After that, the debugger will be called the 2nd time, enter:

```
    reg
    creg
    info gdt
    info idt
    x [addr] // Print value at addr in memory
```

http://bochs.sourceforge.net/doc/docbook/user/internal-debugger.html

https://thestarman.pcministry.com/linux/bochsrc.bxrc.htm

