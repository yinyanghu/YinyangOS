#!/bin/bash
rm os.img
cd kernel
make clean
make
cd ..
cat boot/bootblock kernel/kernel > os_temp.img
./tools/makefs os_temp.img ./user/bb
rm os_temp.img
