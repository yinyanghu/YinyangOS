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

	struct PCB* new_pcb = Proc + new; 
	last_pcb -> next = new_pcb;
	last_pcb = new_pcb;

	new_pcb -> status = STATUS_WAITING;
	new_pcb -> flag = 0;
	
	struct STACK_type *SP = (struct STACK_type*)((int)(new_pcb -> kstack + STACK_SIZE) - sizeof(struct STACK_type));
	//panic("SP = %d\n, kstack = %d\n, size = %d\n", (int)SP, (int)&new_pcb -> kstack[0], sizeof(struct STACK_type));

	SP -> esp = (uint_32)SP + 4;
	new_pcb-> esp = (void *)(SP -> esp);

	SP -> eip = (uint_32)thread;
	SP -> cs = 8;
	SP -> eflags.IF = 1;

}


void init_proc() {

	int i;

	init = Proc;			//Proc[0] --> init
	last_pcb = init;
	for (i = 0; i < MAX_PROC; ++ i)
		Proc[i].flag = 1;

	Create_kthread(process_A);
	Create_kthread(process_B);
	Create_kthread(process_C);
	last_pcb -> next = init -> next;

	current_pcb = init;
}

