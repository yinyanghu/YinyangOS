#include "kernel.h"

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


void Create_uthread(uint_32 file_name) {

	lock();
	int new = Find_Empty_PCB();
	if (new == PROC_FULL)
		panic("Process Table is Full!\n");

	struct PCB *new_pcb = Proc + new;

	new_pcb -> status = STATUS_WAITING;
	new_pcb -> pid = new;
	new_pcb -> flag = FALSE;

	new_pcb -> time_elapsed = 0;

	init_message_pool(new_pcb);

	init_user_page(new_pcb);

	unlock();

	load_init_proc(file_name, new_pcb)

	//kernel stack initialize
	
	//user stack initialize

}

void load_init_proc(uint_32 file_name, struct PCB *pcb) {

	static struct Message m;

	char buf[4200];
	char segment_buf[4200];

	struct ELFHeader *elf;
	struct ProgramHeader *ph, *eph;

	uint_32	va;

	m.type = FM_READ;
	m.fm_msg.file_name = file_name;
	m.fm_msg.buf = buf;
	m.fm_msg.offset = 0;
	m.fm_msg.length = 4096;

	send(FM, &m);
	receive(FM, &m);

	elf = (struct ELFHead *)buf;

	ph = (struct ProgramHead *)((char *)elf + elf -> phoff);
	eph = ph + elf -> phnum;
	for (; ph < eph; ++ ph) {
		va = (uint_32)ph -> vaddr;
		m.type = FM_READ;
		m.fm_msg.file_name = file_name;
		m.fm_msg.buf = segment_buf;
		m.fm_msg.offset = ph -> off;
		m.fm_msg.length = ph -> filesz;

		send(FM, &m);
		receive(FM, &m);

		m.type = MM_ALLOCATE;
		m.mm_msg.target_pcb = pcb;
		m.mm_msg.start = va;
		m.mm_msg.length = va + ph -> memsz;

		send(MM, &m);
		receive(MM, &m);

		copy_from_kernel(pcb, (void *)va, segment_buf, ph -> filesz);
	}

	//eip entry
}

void ProcessManagement(void) {

	

}


