#ifndef __PROCESS_H__
#define __PROCESS_H__

#define MAX_PROC		64
#define PROC_FULL		-1
#define STATUS_WAITING		0
#define STATUS_SLEEPING		-1
#define STATUS_RUNNING		1

struct PCB {
	void *esp;
	uint_32 kstack[STACK_SIZE];
	struct PCB *next, *prev;
	int flag;
	int status;
};

struct PCB Proc[MAX_PROC];

extern struct PCB *current_pcb;
//extern struct PCB *last_pcb;
extern struct PCB *init;

extern boolean need_sched;

void init_proc();
void Create_kthread(void (*)(void));
int Find_Empty_PCB();

#endif
