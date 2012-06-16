#include "kernel.h"

struct PCB Proc[MAX_PROC];

/*
struct EFLAGS {
	uint_32 CF	:1;
	uint_32 pad1	:1;
	uint_32 PF	:1;
	uint_32 pad0_1	:1;
	uint_32 AF	:1;
	uint_32 pad0_2  :1;
	uint_32 ZF	:1;
	uint_32 SF	:1;
	uint_32 TF	:1;
	uint_32 IF	:1;
	uint_32 DF	:1;
	uint_32 OF	:1;
	uint_32 IO_PL	:2;
	uint_32 NT	:1;
	uint_32 pad0_3	:1;
	uint_32 RF	:1;
	uint_32 VM	:1;
	uint_32 offset	:14;
};

struct STACK_type {
	uint_32 esp;
	struct TrapFrame tf;
	uint_32 eip;
	uint_32 cs;
	struct EFLAGS eflags;
};
*/



int Find_Empty_PCB() {
	int_32 i;
	for (i = 1; i < MAX_PROC; ++ i)
		if (Proc[i].flag == TRUE) return i;
	return PROC_FULL;

}



struct PCB* fetch_pcb(pid_t pid) {
	return (Proc + pid);
}


void copy_from_kernel(struct PCB *pcb, void *dest, void *src, int length) {

	uint_32 pde;
	uint_32 pte;
	uint_32 pa;

	struct PageDirectoryEntry	*pdir;
	struct PageTableEntry		*pent;
	void				*ptr;

	pde = ((uint_32)dest) >> 22;
	pte = (((uint_32)dest) >> 12) & 0x3FF;
	pa = ((uint_32)dest) & 0xFFF;


	pdir = (struct PageDirectoryEntry *)((pcb -> cr3).page_directory_base << 12);
	pent = (struct PageTableEntry *)(((pdir + pde) -> page_frame) << 12);

	while (length != 0)
	{
		ptr = (void *)(((pent + pte) -> page_frame) << 12);

		while (pa < PAGE_SIZE)
		{
			*((uint_8 *)ptr + (pa ++)) = *(uint_8 *)(src ++);
			-- length;
			if (length == 0) break;
		}
		pa = 0;
		++ pte;
		if (pte == NR_PTE_ENTRY)
		{
			pte = 0;
			++ pde;
			pent = (struct PageTableEntry *)(((pdir + pde) -> page_frame) << 12);
		}

	}
}

void copy_to_kernel(struct PCB *pcb, void *dest, void *src, int length) {

	uint_32 pde;
	uint_32 pte;
	uint_32 pa;

	struct PageDirectoryEntry	*pdir;
	struct PageTableEntry		*pent;
	void						*ptr;

	pde = ((uint_32)src) >> 22;
	pte = (((uint_32)src) >> 12) & 0x3FF;
	pa = ((uint_32)src) & 0xFFF;

	pdir = (struct PageDirectoryEntry *)((pcb -> cr3).page_directory_base << 12);
	pent = (struct PageTableEntry *)(((pdir + pde) -> page_frame) << 12);

	while (length != 0)
	{
		ptr = (void *)(((pent + pte) -> page_frame) << 12);

		while (pa < PAGE_SIZE)
		{
			*(uint_8 *)(dest ++) = *((uint_8 *)ptr + (pa ++));
			-- length;
			if (length == 0) break;
		}
		pa = 0;
		++ pte;
		if (pte == NR_PTE_ENTRY)
		{
			pte = 0;
			++ pde;
			pent = (struct PageTableEntry *)(((pdir + pde) -> page_frame) << 12);
		}

	}
}



struct PCB *init;

//struct PCB *preempt_proc = NULL;

void init_message_pool(struct PCB *ptr) {

	int_32		i;
	for (i = 0; i < Max_Message_Pool; ++ i)
		(ptr -> Msg_rec_Pool[i]).flag = TRUE;
	ptr -> Msg_rec = NULL;
	ptr -> Msg_ign = NULL;
	Sem_init(&(ptr -> Msg_lock), 0);
}


void Push_Stack_4Byte(uint_8 **Addr, uint_32 Key) {
	*Addr -= 4;
	*((uint_32 *)(*Addr)) = Key;	
}


void Create_kthread(void (*thread)(void)) {

	uint_32		new;

	struct PCB	*new_pcb;

	uint_8		*stack_ptr;
	uint_32		key, i;


	new = Find_Empty_PCB();

	if (new == PROC_FULL)
		panic("Process Table is Full!\n");

	new_pcb = Proc + new;

	new_pcb -> next = init;
	new_pcb -> prev = init -> prev;
	init -> prev -> next = new_pcb;
	init -> prev = new_pcb;
	//last_pcb -> next = new_pcb;
	//last_pcb = new_pcb;




	//Initialize

	new_pcb -> status = STATUS_WAITING;
	new_pcb -> pid = new;
	new_pcb -> flag = FALSE;

	new_pcb -> time_elapsed = 0;


	*(uint_32 *)&(new_pcb -> cr3) = 0;
	(new_pcb -> cr3).page_directory_base = ((uint_32)va_to_pa(kpagedir)) >> 12;


	init_message_pool(new_pcb);

	//printk("kernel stack : %x\n", new_pcb -> kstack);
	stack_ptr = (uint_8 *)((uint_32)(new_pcb -> kstack) + STACK_SIZE);
	//printk("init stack pointer = %x\n", (uint_32)stack_ptr);



	//eflags
	key = (1 << 9); 
	Push_Stack_4Byte(&stack_ptr, key);
	//printk("key = %d", key);
	//printk("stack pointer = %x\n", (uint_32)stack_ptr);
	//printk("eflags = %d\n", *((uint_32 *)(stack_ptr + 4)));
	
	//%cs
	key = 8;
	Push_Stack_4Byte(&stack_ptr, key);
	
	//%eip
	key = (uint_32)thread;
	Push_Stack_4Byte(&stack_ptr, key);
	
	//TrapFrame
	key = 0;
	for (i = 0; i < 9; ++ i)
		Push_Stack_4Byte(&stack_ptr, key);

	//%esp & PCB -> esp
	Push_Stack_4Byte(&stack_ptr, (uint_32)stack_ptr);
	new_pcb -> esp = (void *)((uint_32)stack_ptr);

	
	/*
	for (i = (uint_32)new_pcb -> kstack + STACK_SIZE - 4; i > (uint_32)new_pcb -> kstack + STACK_SIZE - 64; i = i - 4)
		if (*(uint_32 *)i > 0xC0000000)
			printk("%x ", *(uint_32 *)i);
		else
			printk("%d ", *(uint_32 *)i);
	*/

	//printk("\n");
	

	//panic("\nFinish\n");


/*
	struct STACK_type *SP = (struct STACK_type*)(((uint_32)new_pcb -> kstack + STACK_SIZE) - 4 * sizeof(struct STACK_type));
	//panic("SP = %d\n, kstack = %d\n, size = %d\n", (int)SP, (int)&new_pcb -> kstack[0], sizeof(struct STACK_type));

	SP -> esp = (uint_32)SP + 4;
	new_pcb-> esp = (void *)(SP -> esp);

	SP -> eip = (uint_32)thread;
	SP -> cs = 8;
	SP -> eflags.IF = 1;
*/

	/*
	uint_32 esp;
	struct TrapFrame tf;
	uint_32 eip;
	uint_32 cs;
	struct EFLAGS eflags;
	*/
}

struct PCB *current_pcb;

void init_proc() {

	int_32	i;

	init = Proc;			//Proc[0] --> init
	init -> next = init -> prev = init;
	init -> flag = FALSE;
	init -> pid = 0;
	init -> status = STATUS_WAITING;
	init -> time_elapsed = 0;
	init_message_pool(init);

	*(uint_32*)&(init -> cr3) = 0;
	(init -> cr3).page_directory_base = ((uint_32)va_to_pa(kpagedir)) >> 12;
	

	for (i = 1; i < MAX_PROC; ++ i)
		Proc[i].flag = TRUE;

	//Create_kthread(test_ide);

	current_pcb = init;

}

