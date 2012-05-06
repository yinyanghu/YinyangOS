#include "kernel.h"

struct PCB Proc[MAX_PROC];

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



//static
inline int Find_Empty_PCB() {
	int i;
	for (i = 1; i < MAX_PROC; ++ i)
		if (Proc[i].flag == TRUE) return i;
	return PROC_FULL;

}



struct PCB* fetch_pcb(pid_t pid) {
	return (Proc + pid);
}


void copy_from_kernel(struct PCB *pcb_ptr, void *dest, void *src, int len) {
	while (len --)
		*(uint_8 *)(dest ++) = *(uint_8 *)(src ++);
}

void copy_to_kernel(struct PCB *pcb_ptr, void *dest, void *src, int len) {
	while (len --)
		*(uint_8 *)(dest ++) = *(uint_8 *)(src ++);
}

struct PCB *init;



void init_message_pool(struct PCB *ptr) {

	int i;
	for (i = 0; i < Max_Message_Pool; ++ i)
		(ptr -> Msg_rec_Pool[i]).flag = TRUE;
	ptr -> Msg_rec = NULL;
	ptr -> Msg_ign = NULL;
	Sem_init(&(ptr -> Msg_lock), 0);
}





void Create_kthread(void (*thread)(void)) {
	int new = Find_Empty_PCB();
	if (new == PROC_FULL)
		panic("Process Table is Full!\n");

	struct PCB* new_pcb = Proc + new; 
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

	init_message_pool(new_pcb);

	
	struct STACK_type *SP = (struct STACK_type*)((int)(new_pcb -> kstack + STACK_SIZE) - sizeof(struct STACK_type));
	//panic("SP = %d\n, kstack = %d\n, size = %d\n", (int)SP, (int)&new_pcb -> kstack[0], sizeof(struct STACK_type));

	SP -> esp = (uint_32)SP + 4;
	new_pcb-> esp = (void *)(SP -> esp);

	SP -> eip = (uint_32)thread;
	SP -> cs = 8;
	SP -> eflags.IF = 1;

}

struct PCB *current_pcb;

void init_proc() {

	int i;

	init = Proc;			//Proc[0] --> init
	init -> next = init -> prev = init;
	init -> flag = FALSE;
	init -> pid = 0;
	init -> status = STATUS_WAITING;
	
	//last_pcb = init;

	for (i = 1; i < MAX_PROC; ++ i)
		Proc[i].flag = 1;
#ifdef TEST_FUN
	Create_kthread(process_A);
	Create_kthread(process_B);
	Create_kthread(process_C);
#endif

/*
#ifdef TEST_SEMAPHORE
	Create_kthread(producer);
	Create_kthread(consumer);
#endif
*/

#ifdef TEST_TIMER
	Create_kthread(timer_driver_thread);
	TIMER = 1;
	Create_kthread(test_timer);
#endif

#ifdef TEST_TTY
	Create_kthread(tty_driver_thread);
	TTY = 1;
	Create_kthread(test_tty);
#endif

	
	//last_pcb -> next = init -> next;

	current_pcb = init;
}

