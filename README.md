https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf

```
sudo apt install build-essential nasm genisoimage bochs bochs-sdl libsdl2-2.0
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
- Preemptive multi-tasking
- UTF-8 support
- User accounts
- Userland programs that will probably run in ring 0
