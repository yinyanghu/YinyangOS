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

OBJS3 =	../lib/syscall.o \
		../lib/oslib.o \
		fork_bomb.o

OBJS4 =	../lib/syscall.o \
		../lib/oslib.o \
		cc.o

OBJS5 =	../lib/syscall.o \
		../lib/oslib.o \
		dd.o

all:	aa bb fork_bomb cc dd

aa:		$(OBJS1) Makefile
	$(LD) $(LDFLAGS) $(INCLUDE) -N -e main -o aa $(OBJS1)

bb:		$(OBJS2) Makefile
	$(LD) $(LDFLAGS) $(INCLUDE) -N -e main -o bb $(OBJS2)

fork_bomb:		$(OBJS3) Makefile
	$(LD) $(LDFLAGS) $(INCLUDE) -N -e main -o fork_bomb $(OBJS3)

cc:		$(OBJS4) Makefile
	$(LD) $(LDFLAGS) $(INCLUDE) -N -e main -o cc $(OBJS4)

dd:		$(OBJS5) Makefile
	$(LD) $(LDFLAGS) $(INCLUDE) -N -e main -o dd $(OBJS5)

-include *.d

clean:
	rm -f *.d *.o init_proc
	rm -f ../lib/*.d ../lib/*.o
	rm -f aa bb fork_bomb cc dd
