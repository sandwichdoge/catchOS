#!/bin/sh
rm output/catch.iso -f
make debug
if [ "$?" -ne 0 ]; then
    echo 'Error occurred.'
    exit "$?"
fi
make isohw
if [ "$?" -ne 0 ]; then
    echo 'Error occurred.'
    exit "$?"
fi

grub-file --is-x86-multiboot2 output/catch.iso
echo "Is multiboot2 compliant: $?"

cd output
echo '' > serial_debug.txt
#bochs -f bochsrc.txt -q
qemu-system-i386 -accel tcg,thread=single -cpu core2duo -m 128 -no-reboot -drive format=raw,media=cdrom,file=catch.iso -serial file:serial_debug.txt -smp 2 -usb -vga std -object rng-random,id=rng0,filename=/dev/urandom -device virtio-rng-pci,rng=rng0 -monitor stdio -d guest_errors
