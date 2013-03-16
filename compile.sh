#!/bin/bash
rm os.img
cd kernel
make clean
make -j2
cd ..
cat boot/bootblock kernel/kernel > os_temp.img
./tools/makefs os_temp.img ./user/aa
rm os_temp.img
