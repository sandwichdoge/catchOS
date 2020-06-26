#!/bin/sh
make debug
make iso
cd output
bochs -f bochsrc.txt -q
