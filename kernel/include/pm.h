#ifndef __PM_H__
#define __PM_H__

#define PM_LOAD_PROC	0xC0
#define PM_EXIT_PROC	0xC1
#define PM_WAITPID		0xC2
#define PM_FORK			0xC3
#define PM_EXEC			0xC4
#define argv_size		150
#define NR_argv			10


extern pid_t	PM;

/* Structure of a ELF binary header */
struct ELFHeader {
	unsigned int   magic;
	unsigned char  elf[12];
	unsigned short type;
	unsigned short machine;
	unsigned int   version;
	unsigned int   entry;
	unsigned int   phoff;
	unsigned int   shoff;
	unsigned int   flags;
	unsigned short ehsize;
	unsigned short phentsize;
	unsigned short phnum;
	unsigned short shentsize;
	unsigned short shnum;
	unsigned short shstrndx;
};

/* Structure of program header inside ELF binary */
struct ProgramHeader {
	unsigned int type;
	unsigned int off;
	unsigned int vaddr;
	unsigned int paddr;
	unsigned int filesz;
	unsigned int memsz;
	unsigned int flags;
	unsigned int align;
};


struct WaitQueue_type {
	uint_32		pid;
	int_32		wait_for;
};

extern struct WaitQueue_type	WaitQueue[];

extern void asm_do_int80_exit(void);

void ProcessManagement(void);

void init_PM(void);

void Create_uthread(uint_32);

#endif
