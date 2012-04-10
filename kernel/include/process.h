#ifndef __PROCESS_H__
#define __PROCESS_H__

#define MAX_PROC	512
#define PROC_FULL	-1

struct PCB {
	void *esp;
	uint_32 stack[STACK_SIZE];
	struct PCB* next;
	int flag;
};

extern struct PCB Proc[MAX_PROC];

extern struct PCB *current_pcb;
extern struct PCB *tail_pcb;
extern struct PCB *head_pcb;
extern struct PCB *init;

extern boolean need_sched;

void init_proc();
void Create_kthread(void (*)(void));
int Find_Empty_PCB();

#endif
