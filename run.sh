#!/bin/sh
make debug
if [ "$?" -ne 0 ]; then
    echo 'Error occurred.'
    exit "$?"
fi
make iso
if [ "$?" -ne 0 ]; then
    echo 'Error occurred.'
    exit "$?"
fi
cd output
bochs -f bochsrc.txt -q
#qemu-system-i386 -accel tcg,thread=single -cpu core2duo -m 128 -no-reboot -drive format=raw,media=cdrom,file=catch.iso -serial file:serial_debug.txt -smp 1 -usb -vga std
