TESTS_DIR=tests
INCLUDE_DIR=include
KERNEL_DIR=kernel
KBOOT_DIR=$(KERNEL_DIR)/kboot
DRIVERS_DIR=$(KERNEL_DIR)/drivers
INTERRUPT_DIR=$(KERNEL_DIR)/interrupt
MMU_DIR=$(KERNEL_DIR)/mmu
COMMON_DIR=$(KERNEL_DIR)/common
SYS_DIR=$(KERNEL_DIR)/sys
UTILS_DIR=$(KERNEL_DIR)/utils
SHELL_DIR=$(KERNEL_DIR)/shell
TIMER_DIR=$(KERNEL_DIR)/timer
TASKS_DIR=$(KERNEL_DIR)/tasks

CC=gcc
CXX=g++
AS=nasm
ASFLAGS=-f elf32
CFLAGS=-I$(INCLUDE_DIR) -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Wno-unused-parameter \
-D WITH_GRUB_MB -D TARGET_BOCHS -Wall -pedantic
LDFLAGS=-T $(KERNEL_DIR)/link.ld -melf_i386

OBJECTS=$(KBOOT_DIR)/multiboot_header.o $(KBOOT_DIR)/kboot.o \
		$(KBOOT_DIR)/loader.o $(KBOOT_DIR)/gdt.o \
		$(KERNEL_DIR)/kmain.o \
		$(SHELL_DIR)/shell.o \
		$(DRIVERS_DIR)/serial.o $(DRIVERS_DIR)/io.o \
		$(DRIVERS_DIR)/framebuffer.o $(DRIVERS_DIR)/keyboard.o \
		$(DRIVERS_DIR)/pic.o $(DRIVERS_DIR)/pit.o \
		$(INTERRUPT_DIR)/interrupt.o $(INTERRUPT_DIR)/interrupt_hnd.o \
		$(MMU_DIR)/paging.o $(MMU_DIR)/paging_en.o $(MMU_DIR)/kheap.o \
		$(MMU_DIR)/pageframe_alloc.o $(MMU_DIR)/mmu.o $(MMU_DIR)/liballoc.o $(MMU_DIR)/liballoc_hooks.o \
		$(TIMER_DIR)/timer.o \
		$(TASKS_DIR)/tasks.o \
		$(SYS_DIR)/syscall.o \
		$(COMMON_DIR)/kinfo.o \
		$(UTILS_DIR)/string.o $(UTILS_DIR)/debug.o $(UTILS_DIR)/printf.o $(UTILS_DIR)/bitmap.o

all: kernel.elf program

debug: CFLAGS+= -DDEBUG -g
debug: LDFLAGS+= -g
debug: all

test:
	cd tests && make

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
	rm -f $(TIMER_DIR)/*.o
	rm -f $(TASKS_DIR)/*.o
	rm -f $(TESTS_DIR)/*.o
