#include "kernel.h"


void Sem_init(struct Sem_type *sem, int init_token) {
	sem -> token = init_token;
	sem -> queue_tail = 0;
}

/*
struct Sem_type* Sem_new(void) {
	int i;
	for (i = 0; i < Max_Sem; ++ i)
		if (Semaphore[i].flag)
		{
			Semaphore[i].flag = FALSE;
			return (Semaphore + i);
		}
	return NULL;
}
*/

void Sem_P(struct Sem_type *sem)
{
	lock();
	if (sem -> token > 0)
	{
		-- (sem -> token);
		unlock();
	}
	else
	{
		sem -> queue[sem -> queue_tail ++] = current_pcb -> pid;
		current_pcb -> status = STATUS_SLEEPING;
		unlock();
		asm volatile("int $0x80");
	}
}


void Sem_V(struct Sem_type *sem)
{
	lock();
	if (sem -> queue_tail != 0)
	{
		-- (sem -> queue_tail);
		(Proc + (sem -> queue[sem -> queue_tail])) -> status = STATUS_WAITING;
		//preempt_proc = Proc + (sem -> queue[sem -> queue_tail]);
	}
	else
		++ (sem -> token);	
	unlock();
}

