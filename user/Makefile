CC = gcc
AS = gas
LD = ld
CFLAGS = -m32 -g -fno-pic -static -fno-strict-aliasing -fno-builtin -fno-stack-protector -Wall -Werror -O0 -I../include

LDFLAGS = -m elf_i386
OBJCOPY = objcopy
OBJDUMP = objdump
OBJS1 =	../lib/syscall.o \
		../lib/oslib.o \
		aa.o

OBJS2 =	../lib/syscall.o \
		../lib/oslib.o \
		bb.o

all:	aa bb

aa:		$(OBJS1) Makefile
	$(LD) $(LDFLAGS) $(INCLUDE) -N -e main -o aa $(OBJS1)

bb:		$(OBJS2) Makefile
	$(LD) $(LDFLAGS) $(INCLUDE) -N -e main -o bb $(OBJS2)

-include *.d

clean:
	rm -f *.d *.o init_proc
	rm -f ../lib/*.d ../lib/*.o
