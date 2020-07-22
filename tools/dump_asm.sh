source helper.lib

objdump -S "${SCRIPTDIR}"/../kernel.elf -M intel > dis_kernel.s
