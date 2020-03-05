#!/bin/sh
make
make iso
cd output
bochs -f bochsrc.txt -q
