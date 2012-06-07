#include "kernel.h"

struct Message_Pool_type	Message_Pool[Max_Message_Pool];

//static
struct Message_Pool_type* Find_Empty_Message(struct Message_Pool_type *Pool) {
	int i;
	for (i = 0; i < Max_Message_Pool; ++ i)
		if ((Pool + i) -> flag == TRUE) return (Pool + i);
	return NULL;
}

void send(pid_t pid, struct Message *m) {

#ifdef DEBUG
printk("In send, from %d to %d\n", (enter_interrupt == 1) ? MSG_HARD_INTR : current_pcb -> pid, pid);
#endif

	lock();

	struct PCB	*target_pcb = Proc + pid;

	struct Message_Pool_type *new_msg = Find_Empty_Message(target_pcb -> Msg_rec_Pool);

	if (new_msg == NULL)
		panic("\nNo enough space for message passing!\n");

	new_msg -> flag = FALSE;

	new_msg -> msg = *m;

	if (enter_interrupt == 1)
		(new_msg -> msg).src = MSG_HARD_INTR;
	else
		(new_msg -> msg).src = current_pcb -> pid;

	(new_msg -> msg).dest = pid;

	//printk("from %d to %d\n", (new_msg -> msg).src, (new_msg -> msg).dest);

	new_msg -> next = NULL;

	if (target_pcb -> Msg_rec == NULL)
	{
#ifdef DEBUG
printk("Exact NULL %d\n", target_pcb -> pid);
#endif
		target_pcb -> Msg_rec = new_msg;
		target_pcb -> Msg_rec -> prev = target_pcb -> Msg_rec;
	}
	else
	{
		target_pcb -> Msg_rec -> prev -> next = new_msg;
		target_pcb -> Msg_rec -> prev = new_msg;
	}

	unlock();

	Sem_V(&(target_pcb -> Msg_lock));
	
}


void receive(pid_t pid, struct Message *m) {

#ifdef DEBUG
printk("In receive, from %d\n", pid);
#endif

	struct Message_Pool_type	*ptr;

	for (ptr = (current_pcb -> Msg_ign); ptr != NULL && !(pid == ANY || pid == (ptr -> msg).src); ptr = ptr -> next);

	if (ptr != NULL)
	{
		if (ptr == (current_pcb -> Msg_ign))
		{
			if (ptr -> next == NULL)
			{
				current_pcb -> Msg_ign = NULL;

			}
			else
			{
				ptr -> next -> prev = ptr -> prev;
				current_pcb -> Msg_ign = ptr -> next;
			}
		}
		else
		{
			ptr -> prev -> next = ptr -> next;
			ptr -> next -> prev = ptr -> prev;
		}
	}
	else
	{
		while (1)
		{
			Sem_P(&(current_pcb -> Msg_lock));
#ifdef DEBUG
printk("received\n");
#endif

			lock();

			ptr = current_pcb -> Msg_rec;

#ifdef DEBUG
printk("fuck you %d\n", current_pcb -> pid);
printk("Token = %d\n", (current_pcb -> Msg_lock).token);
#endif
			if (current_pcb -> Msg_rec -> next == NULL)
			{
				current_pcb -> Msg_rec = NULL;
			}
			else
			{
				current_pcb -> Msg_rec -> next -> prev = current_pcb -> Msg_rec -> prev;
				current_pcb -> Msg_rec = current_pcb -> Msg_rec -> next;
			}

			unlock();

			if (pid == ANY || pid == (ptr -> msg).src) break;

			if (current_pcb -> Msg_ign == NULL)
			{
				current_pcb -> Msg_ign = ptr;
				current_pcb -> Msg_ign -> next = NULL;
				current_pcb -> Msg_ign -> prev = ptr;
			}
			else
			{
				ptr -> next = NULL;
				current_pcb -> Msg_ign -> prev -> next = current_pcb -> Msg_ign;
				current_pcb -> Msg_ign -> prev = ptr;
			}
		}
	}
	
	*m = ptr -> msg;
	
	lock();
	ptr -> flag = TRUE;
	unlock();
}


