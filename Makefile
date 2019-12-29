CC=gcc
CXX=g++
AS=nasm
ASFLAGS=-f elf32
CFLAGS=-Iinclude -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Wno-unused-parameter \
-D WITH_GRUB_MB
LDFLAGS=-T link.ld -melf_i386

TESTS_DIR=tests
KERNEL_DIR=kernel
KBOOT_DIR=$(KERNEL_DIR)/kboot
DRIVERS_DIR=$(KERNEL_DIR)/drivers
INTERRUPT_DIR=$(KERNEL_DIR)/interrupt
MMU_DIR=$(KERNEL_DIR)/mmu
COMMON_DIR=$(KERNEL_DIR)/common
SYS_DIR=$(KERNEL_DIR)/sys
UTILS_DIR=$(KERNEL_DIR)/utils

OBJECTS=$(KBOOT_DIR)/multiboot_header.o $(KBOOT_DIR)/kboot.o \
		$(KBOOT_DIR)/loader.o $(KBOOT_DIR)/gdt.o \
		$(KERNEL_DIR)/kmain.o \
		$(KERNEL_DIR)/shell/shell.o \
		$(DRIVERS_DIR)/serial.o \
		$(DRIVERS_DIR)/framebuffer.o $(DRIVERS_DIR)/keyboard.o \
		$(INTERRUPT_DIR)/interrupt.o $(INTERRUPT_DIR)/interrupt_hnd.o $(INTERRUPT_DIR)/pic.o \
		$(MMU_DIR)/paging.o $(MMU_DIR)/paging_en.o \
		$(SYS_DIR)/syscall.o \
		$(COMMON_DIR)/io.o $(COMMON_DIR)/kinfo.o \
		$(UTILS_DIR)/string.o

all: kernel.elf program

%.o: %.cpp
	$(CXX) -c $(CFLAGS) $< -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

kernel.elf: $(OBJECTS)
	ld $^ -o $@ $(LDFLAGS)

program: iso/modules/program.s
	$(AS) -f bin $< -o iso/modules/program

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
	rm -f $(KERNEL_DIR)/*.o
	rm -f $(KBOOT_DIR)/*.o
	rm -f $(UTILS_DIR)/*.o
	rm -f $(MMU_DIR)/*.o
	rm -f $(SYS_DIR)/*.o
	rm -f $(INTERRUPT_DIR)/*.o
	rm -f $(DRIVERS_DIR)/*.o
	rm -f $(COMMON_DIR)/*.o
	rm -f $(SHELL_DIR)/*.o
	rm -f $(TESTS_DIR)/*.o