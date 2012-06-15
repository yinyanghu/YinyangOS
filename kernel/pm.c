#include "kernel.h"

pid_t	PM;

static char				buf[4200];
static char				segment_buf[4200];

struct WaitQueue_type	WaitQueue[MAX_PROC];

static uint_32			WQ_ptr;


static void load_init_proc(uint_32, struct PCB *);

static void Wake_Up(pid_t, uint_32);

static void Sleep(pid_t, uint_32);

static void Exit_uthread(pid_t);

static pid_t Fork_uthread(pid_t);

void ProcessManagement(void) {

	static struct Message m;		

	while (TRUE) {
		receive(ANY, &m);
		switch (m.type) {
			case PM_FORK:
				m.pm_msg.p1 = Fork_uthread(m.pm_msg.pid);
				color_printk("fork return = %d\n", m.pm_msg.p1);
				m.type = -1;
				send(m.src, &m);
				break;

			case PM_LOAD_PROC:
				Create_uthread(m.pm_msg.file_name);			
				m.type = -1;
				send(m.src, &m);
				break;
			
			case PM_EXIT_PROC:
				Exit_uthread(m.pm_msg.pid);
				Wake_Up(m.pm_msg.pid, m.pm_msg.p1);
				break;

			case PM_WAITPID:
				Sleep(m.pm_msg.pid, m.pm_msg.p1);
				break;
		}
	}
}

void init_PM(void) {
	WQ_ptr = 0;
}

static inline uint_32 V_to_P(uint_32 va, struct PCB *pcb) {
	static uint_32	pde;
	static uint_32	pte;

	pde = ((uint_32 *)((*(uint_32 *)(&(pcb -> cr3))) & ~0xFFF))[va >> 22];
	pte = ((uint_32 *)(pde & ~0xFFF))[(va >> 12) & 0x3FF];
	return ((pte & ~0xFFF) | (va & 0xFFF));
}

pid_t Fork_uthread(pid_t pid) {

	static uint_32 new;

	static struct PCB *source_pcb, *target_pcb;

	static struct Message m;

	lock();

	//Allocate a new PCB
	source_pcb = Proc + pid;

	new = Find_Empty_PCB();

	if (new == PROC_FULL)
		panic("Process Table is Full!\n");

	target_pcb = Proc + new;

	target_pcb -> pid = new;
	target_pcb -> flag = FALSE;

	target_pcb -> time_elapsed = source_pcb -> time_elapsed;

	init_message_pool(target_pcb);

	unlock();

	//Copy Page Table && Physical Memory
	m.type = MM_COPY;	
	m.mm_msg.source_pcb = source_pcb;
	m.mm_msg.target_pcb = target_pcb;
	send(MM, &m);
	receive(MM, &m);


	lock();

	//initialize Kernel Stack
	target_pcb -> kstack_top = (void *)((uint_32)(target_pcb -> kstack) + STACK_SIZE);
	*((uint_32 *)(target_pcb -> kstack_top) - 1) = *((uint_32 *)(source_pcb -> kstack_top) - 1);

	//panic("Kernel stack = %x, %x\n", (uint_32)target_pcb -> kstack_top, *((uint_32 *)(target_pcb -> kstack_top) - 1));


	//Initialize current_pcb -> esp
	target_pcb -> esp = (void *)((uint_32)target_pcb -> kstack_top - 4);
	//panic("ESP = %x %x\n", (uint_32)target_pcb -> esp, *(uint_32 *)(target_pcb -> esp));




	//save CR3
	*(uint_32 *)&(target_pcb -> cr3) = 0;
	(target_pcb-> cr3).page_directory_base = ((uint_32)va_to_pa(target_pcb -> pagedir)) >> 12;


	//eax <-- return address
	static uint_32		eax_addr;

	eax_addr = *((uint_32 *)target_pcb -> kstack_top - 1) + 4 * 7;
	eax_addr = V_to_P(eax_addr, target_pcb);
	*((uint_32 *)eax_addr) = 0;
	
	
	/*
	static int_32		i;
	for (i = 0; i < 9; ++ i)
		printk("address = %x, key = %x\n", eax_addr + i * 4, *((uint_32 *)eax_addr + i));
		*/

	//panic("stop!");


	target_pcb -> next = init;
	target_pcb -> prev = init -> prev;
	init -> prev -> next = target_pcb;
	init -> prev = target_pcb;
	target_pcb -> status = STATUS_WAITING;

	unlock();
	
	printk("Ready to schedule fork process.........................OK!\n");
	return new;
}



static void Wake_Up(pid_t pid, uint_32 exitcode) {

	static uint_32	i;
	static struct Message	m;

	lock();

	printk("enter Wake_Up..................OK!\n");

	i = 0;
	m.type = -1;
	m.pm_msg.p1 = exitcode;
	while (i < WQ_ptr)
	{
//		printk("%d %d\n", i, WQ_ptr);
		if (WaitQueue[i].wait_for == pid)
		{
			send(WaitQueue[i].pid, &m);
			if (WQ_ptr > 1)
			{
				WaitQueue[i].pid = WaitQueue[WQ_ptr - 1].pid;
				WaitQueue[i].wait_for = WaitQueue[WQ_ptr - 1].wait_for;
			}
			-- WQ_ptr;
		}
		else
			++ i;
	}

	printk("leave Wake_Up..................OK!\n");
	unlock();
}

static void Sleep(pid_t pid, uint_32 wait_for) {
	lock();
	WaitQueue[WQ_ptr].pid = pid;
	WaitQueue[WQ_ptr].wait_for = wait_for;
	++ WQ_ptr;
	unlock();
}

static void Exit_uthread(pid_t pid) {

	static struct PCB		*pcb_ptr;

	static struct Message	m;

	lock();	

	pcb_ptr = Proc + pid;

	pcb_ptr -> status = STATUS_SLEEPING;
	pcb_ptr -> flag = TRUE;
	pcb_ptr -> prev -> next = pcb_ptr -> next;
	pcb_ptr -> next -> prev = pcb_ptr -> prev;

	unlock();
//	exit_page(pcb_ptr);
	
//	printk("status = %d, flag = %d\n", pcb_ptr -> status, pcb_ptr -> flag);
	
	m.type = MM_EXIT_PROC;
	m.mm_msg.target_pcb = pcb_ptr;
	send(MM, &m);
	receive(MM, &m);
	

	printk("Exit_uthread........................OK!\n");

}

void Create_uthread(uint_32 file_name) {

	static uint_32 new;
	
	static struct PCB *new_pcb;

	lock();
	
	new = Find_Empty_PCB();

	if (new == PROC_FULL)
		panic("Process Table is Full!\n");

	new_pcb = Proc + new;

	new_pcb -> pid = new;
	new_pcb -> flag = FALSE;

	new_pcb -> time_elapsed = 0;

	init_message_pool(new_pcb);

	printk("New pid = %d\n", new_pcb -> pid);


	init_user_page(new_pcb);

	unlock();

	load_init_proc(file_name, new_pcb);

	//initialize kernel stack
	new_pcb -> kstack_top = (void *)((uint_32)(new_pcb -> kstack) + STACK_SIZE);
	//empty stack
	
	//initialize user stack
	//	Completed in load_init_proc

	//Ready to Schedule
	
	printk("ready to schedule................!!!!\n");
	lock();

	new_pcb -> next = init;
	new_pcb -> prev = init -> prev;
	init -> prev -> next = new_pcb;
	init -> prev = new_pcb;
	new_pcb -> status = STATUS_WAITING;

	unlock();
}


static void load_init_proc(uint_32 file_name, struct PCB *pcb) {

	static struct Message m;

	struct ELFHeader *elf;
	struct ProgramHeader *ph, *eph;

	static uint_32	va;

	printk("enter load_init_proc ...........Successful\n");
	printk("send FM read request ...........Successful\n");
	m.type = FM_READ;
	m.fm_msg.file_name = file_name;
	m.fm_msg.buf = buf;
	m.fm_msg.offset = 0;
	m.fm_msg.length = 4096;

	send(FM, &m);
	//panic("sb");
	receive(FM, &m);
	printk("received ...........Successful\n");
	//panic("sb\n");
	/*
	static int_32	j;
	for (j = 0; j < 4096; ++ j)
		if (buf[j] != 0)
			printk("%c", buf[j]);
			*/


	printk("Read ELF header\n"); 
	elf = (struct ELFHeader *)buf;

	ph = (struct ProgramHeader *)((char *)elf + elf -> phoff);
	eph = ph + elf -> phnum;
	printk("begin %x, end %x\n", ph, eph);
	//static uint_32 total = 0;
	for (; ph < eph; ++ ph) {
		//printk("****************************\n");
		//printk("current: %x\n", ph);
		va = (uint_32)ph -> vaddr;

		//printk("virtual address: %x\n", va);
		m.type = FM_READ;
		m.fm_msg.file_name = file_name;
		m.fm_msg.buf = segment_buf;
		m.fm_msg.offset = ph -> off;
		m.fm_msg.length = ph -> filesz;


		//printk("send FM read request, file name = %d, offset = %d, length = %d\n", file_name, ph -> off, ph -> filesz);
		send(FM, &m);
		receive(FM, &m);

		//printk("received from FM ...........Successful\n");

		/*
		for (j = 0; j < 4096; ++ j)
			if (segment_buf[j] != 0)
				printk("%c", segment_buf[j]);
				*/

		m.type = MM_ALLOCATE;
		m.mm_msg.target_pcb = pcb;

		/* Virtual Address Space = [va, va + memsz) */
		m.mm_msg.start = va;
		m.mm_msg.length = ((ph -> memsz) >> 12) + 1;	

		printk("allocate memory: [%x %x), file size = %d\n", va, va + ph -> memsz, ph -> filesz);
		//total += ph -> memsz;

		//printk("send MM read request, target_pcb = %d, va = %d, length = %d\n", pcb -> pid, va,  ph -> memsz);

		send(MM, &m);
		receive(MM, &m);

		//printk("received from MM .........Successful\n");
		copy_from_kernel(pcb, (void *)va, segment_buf, ph -> filesz);
		//printk("copy program segments from kernel to user space.............Successful\n");
	}
	//panic("stop!");

	printk("Finish analyse ELF header\n");
	//initialize user stack
	static uint_32	stack_ptr = 0xC0000000;
	static uint_32	key;

	key = (uint_32)asm_do_int80_exit;
	stack_ptr -= 4;
	copy_from_kernel(pcb, (void *)stack_ptr, (void *)&key, 4);

	printk("Yang yang da shen!\n");

	//eflags
	key = (1 << 9);
	stack_ptr -= 4;
	copy_from_kernel(pcb, (void *)stack_ptr, (void *)&key, 4);

	//%cs	
	key = 8;
	stack_ptr -= 4;
	copy_from_kernel(pcb, (void *)stack_ptr, (void *)&key, 4);

	//%eip
	key = elf -> entry;
	stack_ptr -= 4;
	copy_from_kernel(pcb, (void *)stack_ptr, (void *)&key, 4);

	//TrapFrame
	static uint_32 i;
	key = 0;
	for (i = 0; i < 9; ++ i)
	{
		stack_ptr -= 4;
		copy_from_kernel(pcb, (void *)stack_ptr, (void *)&key, 4);
	}

	//Initialize current_pcb -> esp

	stack_ptr -= 4;
	key = stack_ptr + 4;
	copy_from_kernel(pcb, (void *)stack_ptr, (void *)&key, 4);
	pcb -> esp = (void *)stack_ptr;
}

