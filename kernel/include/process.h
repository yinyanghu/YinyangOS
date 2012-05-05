#ifndef __PROCESS_H__
#define __PROCESS_H__


#define MAX_PROC		64
#define PROC_FULL		-1
#define STATUS_WAITING		0
#define STATUS_SLEEPING		-1
#define STATUS_RUNNING		1

struct PCB {
	void				*esp;
	uint_32				kstack[STACK_SIZE];
	struct PCB			*next, *prev;
	boolean				flag;
	int				status;
	pid_t				pid;

	struct Sem_type			Msg_lock;
	struct Message_Pool_type	Msg_rec_Pool[Max_Message_Pool];	
	//struct Message_Pool_type	Msg_ign_Pool[Max_Message_Pool];
	struct Message_Pool_type	*Msg_rec, *Msg_ign;
};

extern struct PCB Proc[MAX_PROC];

extern struct PCB *current_pcb;
//extern struct PCB *last_pcb;
extern struct PCB *init;

extern boolean need_sched;





void init_proc(void);


int Find_Empty_PCB(void);
void init_message_pool(struct PCB *ptr);
void Create_kthread(void (*)(void));



struct PCB* find_next_live_process(void);
void schedule(void);




#endif
