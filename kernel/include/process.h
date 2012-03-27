#ifndef __PROCESS_H__
#define __PROCESS_H__

struct PCB {
	void *esp;
};

extern struct PCB *current_pcb;
extern boolean need_sched;

void init_proc();

#endif
