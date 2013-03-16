CC = gcc
AS = gas
LD = ld
CFLAGS = -m32 -g -fno-pic -static -fno-strict-aliasing -fno-builtin -fno-stack-protector -Wall -Werror -O0 -I../include
ASFLAGS = -m32

LDFLAGS = -m elf_i386
OBJCOPY = objcopy
OBJDUMP = objdump

all:	syscall.o oslib.o

clean:
	rm -f oslib.o syscall.o
