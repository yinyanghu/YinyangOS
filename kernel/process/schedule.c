#include "kernel.h"
/* You interrupt handler (use add_intr_handle to register)
   just need to set "need_sched" to be TRUE,
   schedule() will be called each time before interrupt return. */

boolean	need_sched = TRUE;

struct PCB* find_next_live_process(void) {
	struct PCB *p;
	/*
	if (preempt_proc != NULL)
	{
		p = preempt_proc;	
		preempt_proc = NULL;
	}
	else*/
	for (p = current_pcb -> next; p != NULL && p -> status != STATUS_WAITING; p = p -> next);
	return p;
}

void schedule(void) {
	//lock();
	if (need_sched)
	{
		if (current_pcb -> status == STATUS_RUNNING)
			current_pcb -> status = STATUS_WAITING;
		current_pcb = find_next_live_process();
//		printk("%d\n", current_pcb -> pid);
		current_pcb -> status = STATUS_RUNNING;
		printk("%d", current_pcb -> pid);
//		printk("CR3 = %x\n", *((uint_32 *)(&(current_pcb -> cr3))));
//		save_cr3(&(current_pcb -> cr3));
/*
		if (*((uint_32 *)&(Proc[6].cr3)) < 1000) 
		{
			int i;
			for (i = 0; i < 8; ++ i)
				printk("Process %d, CR3 = %x\n", i, *((uint_32 *)(&((Proc + i) -> cr3))));
				
			panic("why, why?\n");
		}
		printk("Saving CR3...........\n");
*/
		asm volatile ("movl %0, %%cr3" : : "r"(*((uint_32 *)(&(current_pcb -> cr3)))));
//		printk("Saving CR3...........Successful!\n");
		need_sched = FALSE;
	}
	//unlock();
}

