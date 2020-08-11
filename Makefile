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
POWER_DIR=$(KERNEL_DIR)/power

CC=gcc
CXX=g++
AS=nasm
ASFLAGS=-f elf32
CFLAGS=-I$(INCLUDE_DIR) -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
-Wno-unused-parameter -Wno-language-extension-token -Wno-zero-length-array \
-D WITH_GRUB_MB -D TARGET_BOCHS -D DEBUG_LIBALLOC \
-Wall -Wextra -pedantic
LDFLAGS=-T $(KERNEL_DIR)/link.ld -melf_i386

OBJECTS=$(KBOOT_DIR)/multiboot_header.o $(KBOOT_DIR)/kboot.o \
		$(KBOOT_DIR)/loader.o $(KBOOT_DIR)/gdt.o \
		$(KERNEL_DIR)/kmain.o \
		$(SHELL_DIR)/shell.o $(SHELL_DIR)/font.o $(SHELL_DIR)/bmp.o \
		$(DRIVERS_DIR)/serial.o $(DRIVERS_DIR)/io.o \
		$(DRIVERS_DIR)/framebuffer.o $(DRIVERS_DIR)/keyboard.o \
		$(DRIVERS_DIR)/pic.o $(DRIVERS_DIR)/pit.o \
		$(DRIVERS_DIR)/acpi/fadt.o $(DRIVERS_DIR)/acpi/acpi.o \
		$(DRIVERS_DIR)/cpuid.o $(DRIVERS_DIR)/svga.o \
		$(INTERRUPT_DIR)/interrupt.o $(INTERRUPT_DIR)/interrupt_hnd.o \
		$(MMU_DIR)/paging.o $(MMU_DIR)/paging_en.o $(MMU_DIR)/kheap.o \
		$(MMU_DIR)/pageframe_alloc.o $(MMU_DIR)/mmu.o $(MMU_DIR)/liballoc.o $(MMU_DIR)/liballoc_hooks.o \
		$(TIMER_DIR)/timer.o \
		$(TASKS_DIR)/tasks.o $(TASKS_DIR)/cpu_switch_to.o $(TASKS_DIR)/sem.o \
		$(SYS_DIR)/syscall.o \
		$(POWER_DIR)/shutdown.o $(POWER_DIR)/reboot.o \
		$(COMMON_DIR)/kinfo.o $(COMMON_DIR)/panic.o \
		$(UTILS_DIR)/hashtable.o \
		$(UTILS_DIR)/string.o $(UTILS_DIR)/debug.o $(UTILS_DIR)/printf.o $(UTILS_DIR)/bitmap.o \
		$(UTILS_DIR)/queue.o $(UTILS_DIR)/list.o $(UTILS_DIR)/spinlock.o $(UTILS_DIR)/maths.o

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
	-o output/catch.iso \
	iso

isohw: kernel.elf
	cp kernel.elf iso/boot/
	grub-mkrescue -o output/catch.iso iso

clean:
	rm -rf *.o *.elf
	find . -type f -name '*.o' -delete
