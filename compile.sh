#!/bin/bash
cd kernel
make
cd ..
cat boot/bootblock kernel/kernel > os.img
