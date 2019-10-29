all: loader.o
	ld -T link.ld -melf_i386 $? -o kernel.elf

loader.o: loader.s
	nasm -f elf32 $<

geniso:
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
	rm -rf *.o