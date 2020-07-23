SCRIPTDIR=$(dirname "$0")
objdump -S "${SCRIPTDIR}"/../kernel.elf -M intel > dis_kernel.s
