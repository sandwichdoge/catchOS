source helper.lib
echo "$SCRIPTDIR"
objdump "${SCRIPTDIR}"/../kernel.elf -h
