#include "kernel.h"

pid_t	PM;

void load_init_proc(uint_32 file_name, struct PCB *pcb) {

	static struct Message m;

	char buf[4200];
	char segment_buf[4200];

	struct ELFHeader *elf;
	struct ProgramHeader *ph, *eph;

	uint_32	va;

	printk("enter load_init_proc ...........Successful\n");
	printk("send FM read request ...........Successful\n");
	m.type = FM_READ;
	m.fm_msg.file_name = file_name;
	m.fm_msg.buf = buf;
	m.fm_msg.offset = 0;
	m.fm_msg.length = 4096;

	send(FM, &m);
	receive(FM, &m);
	printk("received ...........Successful\n");
 
	printk("Read ELF header\n"); 
	elf = (struct ELFHeader *)buf;

	ph = (struct ProgramHeader *)((char *)elf + elf -> phoff);
	eph = ph + elf -> phnum;
	printk("begin %x, end %x\n", ph, eph);
	for (; ph < eph; ++ ph) {
		printk("****************************\n");
		printk("current: %x\n", ph);
		va = (uint_32)ph -> vaddr;

		printk("virtual address: %x\n", va);
		m.type = FM_READ;
		m.fm_msg.file_name = file_name;
		m.fm_msg.buf = segment_buf;
		m.fm_msg.offset = ph -> off;
		m.fm_msg.length = ph -> filesz;

		printk("send FM read request, file name = %d, offset = %d, length = %d\n", file_name, ph -> off, ph -> filesz);
		send(FM, &m);
		receive(FM, &m);

		printk("received from FM ...........Successful\n");

		m.type = MM_ALLOCATE;
		m.mm_msg.target_pcb = pcb;
		m.mm_msg.start = va;
		m.mm_msg.length = ph -> memsz;

		printk("send MM read request, target_pcb = %d, va = %d, length = %d\n", pcb -> pid, va,  ph -> memsz);

		send(MM, &m);
		receive(MM, &m);

		printk("received from MM .........Successful\n");
		copy_from_kernel(pcb, (void *)va, segment_buf, ph -> filesz);
		printk("copy program segments from kernel to user space.............Successful\n");
	}

	printk("Finish analyse ELF header\n");
	//initialize user stack
	uint_32	stack_ptr = 0xC0000000 - 4;
	uint_32 key;

	//eflags
	key = (1 << 9);
	copy_from_kernel(pcb, (void *)stack_ptr, (void *)&key, 4);
	stack_ptr -= 4;

	//%cs	
	key = 8;
	copy_from_kernel(pcb, (void *)stack_ptr, (void *)&key, 4);
	stack_ptr -= 4;

	//%eip
	key = elf -> entry;
	copy_from_kernel(pcb, (void *)stack_ptr, (void *)&key, 4);
	stack_ptr -= 4;

	//TrapFrame
	uint_32 i;
	key = 0;
	for (i = 0; i < 9; ++ i)
	{
		copy_from_kernel(pcb, (void *)stack_ptr, (void *)&key, 4);
		stack_ptr -= 4;
	}

	//Initialize current_pcb -> esp
	pcb -> esp = (void *)stack_ptr;

	key = stack_ptr + 4;
	copy_from_kernel(pcb, (void *)stack_ptr, (void *)&key, 4);
	stack_ptr -= 4;
}

void Create_uthread(uint_32 file_name) {

	lock();
	int new = Find_Empty_PCB();
	if (new == PROC_FULL)
		panic("Process Table is Full!\n");

	struct PCB *new_pcb = Proc + new;

	new_pcb -> pid = new;
	new_pcb -> flag = FALSE;

	new_pcb -> time_elapsed = 0;

	init_message_pool(new_pcb);

	printk("New pid = %d\n", new_pcb -> pid);


	init_user_page(new_pcb);

	unlock();

	load_init_proc(file_name, new_pcb);

	//initialize kernel stack
	new_pcb -> kstack_top = (void *)((uint_32)(new_pcb -> kstack) + STACK_SIZE - 4);
	//empty stack
	
	//initialize user stack
	//	Completed in load_init_proc

	//Ready to Schedule
	lock();

	new_pcb -> next = init;
	new_pcb -> prev = init -> prev;
	init -> prev -> next = new_pcb;
	init -> prev = new_pcb;
	new_pcb -> status = STATUS_WAITING;

	unlock();
}


void ProcessManagement(void) {

	static struct Message m;		

	while (TRUE) {
		receive(ANY, &m);
		if (m.type == PM_LOAD_PROC)
		{
			Create_uthread(m.pm_msg.file_name);			
			m.type = -1;
			send(m.src, &m);
		}
	}
}


