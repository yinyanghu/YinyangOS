#!/bin/bash
rm os.img osfs.img
cd kernel
make clean
make
cd ..
cat boot/bootblock kernel/kernel > os.img
./makefs os.img init_proc
