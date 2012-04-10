#!/bin/bash
cd kernel
make clean
make
cd ..
cat boot/bootblock kernel/kernel > os.img
