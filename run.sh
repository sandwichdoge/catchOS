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
