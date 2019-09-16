make clean
make
make dist
sh-elf-objcopy -O binary rvmscd.elf rvmscd.bin
$KOS_BASE/utils/scramble/scramble rvmscd.bin 1ST_READ.bin

