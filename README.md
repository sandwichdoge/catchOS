## catchOS
A unix-like monolithic kernel, plus some userland applications (non POSIX-compliant).

https://linux-kernel-labs.github.io

```
sudo apt install build-essential nasm genisoimage bochs bochs-sdl libsdl2-2.0 qemu-system-x86
git submodule update --init
```

```
./run.sh
```

kboot : Loaded by GRUB, still in 16-bit real mode.

kmain : We're now in 32-bit protected mode.

The kernel will have
- Virtual memory
- Heap allocation
- Cooperative multi-tasking
- UTF-8 support
- User accounts
- Userland programs that will probably run in ring 0
