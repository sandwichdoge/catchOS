CC=gcc
AS=nasm
ASFLAGS=-f elf32
CFLAGS=-m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra
LDFLAGS=-T link.ld -melf_i386
OBJECTS=loader.o kmain.o framebuffer.o io.o serial.o

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