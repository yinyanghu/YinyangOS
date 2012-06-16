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

	lock();

	struct PCB	*target_pcb;

	struct Message_Pool_type *new_msg;
		
	target_pcb = Proc + pid;
		
	new_msg = Find_Empty_Message(target_pcb -> Msg_rec_Pool);

	if (new_msg == NULL)
		panic("\nNo enough space for message passing!\n");

	new_msg -> flag = FALSE;

	new_msg -> msg = *m;

	if (enter_interrupt == 1)
		(new_msg -> msg).src = MSG_HARD_INTR;
	else
		(new_msg -> msg).src = current_pcb -> pid;

	(new_msg -> msg).dest = pid;


//	if (target_pcb -> pid == 6)
//		color_printk("send from %d to %d\n", current_pcb -> pid, target_pcb -> pid);
	//printk("from %d to %d\n", (new_msg -> msg).src, (new_msg -> msg).dest);

	//new_msg -> next = NULL;

	if (target_pcb -> Msg_rec == NULL)
	{
		target_pcb -> Msg_rec = new_msg;
		target_pcb -> Msg_rec -> prev = target_pcb -> Msg_rec;
		target_pcb -> Msg_rec -> next = target_pcb -> Msg_rec;
	}
	else
	{
		new_msg -> prev = target_pcb -> Msg_rec -> prev;
		new_msg -> next = target_pcb -> Msg_rec;

		target_pcb -> Msg_rec -> prev -> next = new_msg;
		target_pcb -> Msg_rec -> prev = new_msg;
	}

	unlock();

	//color_printk("Semephore Wake up %d, lock = %d, qeueu = %d, %d\n", target_pcb -> pid, target_pcb -> Msg_lock. token, target_pcb -> Msg_lock.queue_tail, target_pcb -> Msg_lock.queue[0]);
	Sem_V(&(target_pcb -> Msg_lock));
//	if (target_pcb -> pid == 6)
//	color_printk("Lock %d = %d\n", target_pcb -> pid, target_pcb -> Msg_lock.token);
	
}


void receive(pid_t pid, struct Message *m) {

	struct Message_Pool_type	*ptr;

	if (current_pcb -> Msg_ign == NULL)
	{
		ptr = NULL;
	}
	else
	{
		ptr = current_pcb -> Msg_ign;
		if (!(pid == ANY || pid == (ptr -> msg).src))
		{
			for (ptr = (current_pcb -> Msg_ign -> next); ptr != current_pcb -> Msg_ign && !(pid == ANY || pid == (ptr -> msg).src); ptr = ptr -> next);
			if (ptr == current_pcb -> Msg_ign)
				ptr = NULL;
		}
	}

	if (ptr != NULL)
	{
//		color_printk("Ignored Message!\n");
		if (ptr == (current_pcb -> Msg_ign))
		{
			if (ptr -> next == current_pcb -> Msg_ign)
			{
				current_pcb -> Msg_ign = NULL;

			}
			else
			{
				ptr -> next -> prev = ptr -> prev;
				ptr -> prev -> next = ptr -> next;
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

			if (ptr == NULL) panic("Message receive is empty, semephore is wrong!\n");

			/*
			if (current_pcb -> pid == 6)
			{
			printk("See Message !!! %d %d\n", ptr -> msg. src, ptr -> msg.dest);
			printk("want message from %d\n", pid);
			}
			*/

			if (current_pcb -> Msg_rec -> next == current_pcb -> Msg_rec)
			{
				current_pcb -> Msg_rec = NULL;
			}
			else
			{
				current_pcb -> Msg_rec -> next -> prev = current_pcb -> Msg_rec -> prev;
				current_pcb -> Msg_rec -> prev -> next = current_pcb -> Msg_rec -> next;
				current_pcb -> Msg_rec = current_pcb -> Msg_rec -> next;
			}


			if (pid == ANY || pid == (ptr -> msg).src)
			{
				unlock();
				break;
			}

			if (current_pcb -> Msg_ign == NULL)
			{
				current_pcb -> Msg_ign = ptr;
				current_pcb -> Msg_ign -> next = ptr;
				current_pcb -> Msg_ign -> prev = ptr;
			}
			else
			{
				ptr -> next = current_pcb -> Msg_ign;
				ptr -> prev = current_pcb -> Msg_ign -> prev;
				current_pcb -> Msg_ign -> prev -> next = ptr;
				current_pcb -> Msg_ign -> prev = ptr;
			}
			unlock();
		}
	}
	
	*m = ptr -> msg;
	
	lock();
	ptr -> flag = TRUE;
	unlock();
}


