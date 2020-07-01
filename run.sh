#!/bin/sh
make debug
if [ "$?" -ne 0 ]; then
    echo 'Error occurred.'
fi
make iso
cd output
bochs -f bochsrc.txt -q
