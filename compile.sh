#!/bin/bash
rm os.img osfs.img
cd kernel
make clean
make
cd ..
cat boot/bootblock kernel/kernel > os_temp.img
./tools/makefs os_temp.img ./user/init_proc
rm os_temp.img
