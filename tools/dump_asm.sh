SCRIPTDIR=$(dirname "$0")
objdump -D "${SCRIPTDIR}"/../kernel.elf -M intel > dis_kernel.s
