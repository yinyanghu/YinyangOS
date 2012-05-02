bootblock: start.S main.c asm.h boot.h
	gcc -c start.S -g
	gcc -c -O1 main.c -m32 -g
	ld -e start -Ttext 0x7C00 -m elf_i386 start.o main.o
	objcopy -S -O binary -j .text a.out bootblock
	./genboot.pl bootblock

clean:
	rm -rf bootblock a.out *.o
