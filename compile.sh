#!/bin/bash
rm os.img osfs.img
cd kernel
make clean
make
cd ..
cat boot/bootblock kernel/kernel > os.img
./makefs os.img aaa bbb ccc
