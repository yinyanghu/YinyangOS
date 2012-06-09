CC = gcc
AS = gas
LD = ld
CFLAGS = -m32 -g -fno-pic -static -fno-strict-aliasing -fno-builtin -fno-stack-protector -Wall -Werror

LDFLAGS = -m elf_i386
OBJCOPY = objcopy
OBJDUMP = objdump
OBJS = init_proc.o 
	
init_proc:	$(OBJS) Makefile
	$(LD) $(LDFLAGS) $(INCLUDE) -N -e main -o init_proc $(OBJS)

-include *.d

clean:
	rm -f *.d *.o init_proc
