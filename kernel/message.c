#include "kernel.h"

struct Message_Pool_type	Message_Pool[Max_Message_Pool];

//static
struct Message_Pool_type* Find_Empty_Message(struct Message_Pool_type *Pool) {
	int i;
	for (i = 0; i < Max_Message_Pool; ++ i)
		if ((Pool + i) -> flag == TRUE) return (Pool + i);
	return NULL;
}

void send(pid_t pid, struct Message_type *m) {

	lock();

	struct PCB	*target_pcb = Proc + pid;

	struct Message_Pool_type *new_msg = Find_Empty_Message(target_pcb -> Msg_rec_Pool);
	if (new_msg == NULL)
		panic("\nNo enough space for message passing!\n");

	new_msg -> flag = FALSE;

	new_msg -> msg = *m;

	if (enter_interrupt)
		(new_msg -> msg).src = PID_Hardware_IRQ;
	else
		(new_msg -> msg).src = current_pcb -> pid;

	(new_msg -> msg).dest = pid;

	if (target_pcb -> Msg_rec == NULL)
	{
		target_pcb -> Msg_rec = new_msg;
		target_pcb -> Msg_rec -> next = NULL;
		target_pcb -> Msg_rec -> prev = new_msg;
	}
	else
	{
		new_msg -> next = NULL;
		target_pcb -> Msg_rec -> prev -> next = new_msg;
		target_pcb -> Msg_rec -> prev = new_msg;
	}

	unlock();

	Sem_V(&(target_pcb -> Msg_lock));
	
}


void receive(pid_t pid, struct Message_type *m) {

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

			lock();

			ptr = current_pcb -> Msg_rec;

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


