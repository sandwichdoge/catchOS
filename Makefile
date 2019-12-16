CC=gcc
AS=nasm
ASFLAGS=-f elf32
CFLAGS=-Iinclude -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Wno-unused-parameter
LDFLAGS=-T link.ld -melf_i386
OBJECTS=kboot/loader.o kboot/kboot.o \
		kboot/gdt.o \
		kmain.o serial.o \
		controller.o \
		interrupt.o interrupt_hnd.o pic.o \
		keyboard.o \
		framebuffer.o io.o \
		utils/string.o

all: kernel.elf

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

kernel.elf: $(OBJECTS)
	ld $(LDFLAGS) $^ -o $@

iso: kernel.elf
	cp kernel.elf iso/boot/
	genisoimage -R \
	-b boot/grub/stage2_eltorito \
	-no-emul-boot \
	-boot-load-size 4 \
	-A os \
	-input-charset utf8 \
	-quiet \
	-boot-info-table \
	-o output/os.iso \
	iso

clean:
	rm -rf *.o *.elf
	rm -f kboot/*.o
	rm -f utils/*.o
	rm -f memory/*.o