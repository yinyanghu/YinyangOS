#include "kernel.h"

struct EFLAGS {
	uint_32 CF      :1;
	uint_32 pad1    :1;
	uint_32 PF      :1;
	uint_32 pad0_1  :1;                                                                                                                        
	uint_32 AF      :1;
	uint_32 pad0_2  :1;
	uint_32 ZF      :1;
	uint_32 SF      :1;
	uint_32 TF      :1;
	uint_32 IF      :1;
	uint_32 DF      :1;
	uint_32 OF      :1;
	uint_32 IO_PL   :2;
	uint_32 NT      :1;
	uint_32 pad0_3  :1;
	uint_32 RF      :1;
	uint_32 VM      :1;
	uint_32 offset  :14;
};


struct STACK_type {
	uint_32 esp;
	struct TrapFrame tf;
	uint_32 eip;
	uint_32 cs;
	struct EFLAGS eflags;
};

inline int Find_Empty_PCB() {
	int i;
	for (i = 1; i < MAX_PROC; ++ i)
		if (Proc[i].flag == 1) return i;
	return PROC_FULL;

}

void Create_kthread(void (*thread)(void)) {
	int new = Find_Empty_PCB();
	if (new == PROC_FULL)
		panic("Process Table is Full!\n");

	printk("%d", new);
	(Proc + new) -> flag = 0;
	
//	(Proc + new) -> next = init;
//	last_pcb -> next = Proc + new;
//	last_pcb = Proc + new;
	struct STACK_type *SP = (struct STACK_type*)(((Proc + new) -> stack) - sizeof(struct STACK_type));

	//printk("%d\n", SP);
	SP -> esp = (uint_32)&SP -> esp + 4;
	(Proc + new) -> esp = (void *)SP -> esp;

//printk("yyy\n");
	SP -> eip = (uint_32)thread;
	SP -> cs = 8;
	SP -> eflags.IF = 1;
	SP -> tf.irq = 1001;

}


void init_proc() {

	int i;

	init = Proc;
	init -> next = (Proc + 1);
	last_pcb = init;
	for (i = 0; i < MAX_PROC; ++ i)
		Proc[i].flag = 1;


	//current_pcb = (Proc + 1);
	printk("yinyangh\n");

	
	Create_kthread(process_A);
	//printk("%d %d\n", (Proc + 1), &((Proc + 1) -> esp));
	Create_kthread(process_B);
	Create_kthread(process_C);
	(Proc + 1) -> next =  Proc + 2;
	(Proc + 2) -> next = Proc + 3;
	(Proc + 3) -> next = Proc + 1;
	//last_pcb -> next = (Proc + 1);
	current_pcb = init; 
//	panic("stop");

	
}

