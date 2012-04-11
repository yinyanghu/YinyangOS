#include "kernel.h"

struct PCB *current_pcb = NULL;
struct PCB *last_pcb;
struct PCB *init;

/* You interrupt handler (use add_intr_handle to register)
   just need to set "need_sched" to be TRUE,
   schedule() will be called each time before interrupt return. */
boolean need_sched = TRUE;

struct PCB* find_next_live_process() {
	struct PCB *p;
	for (p = current_pcb -> next; p != NULL && p -> status != STATUS_WAITING; p = p -> next);
	return p;
}

void
schedule(void) {
	if (need_sched)
	{
		current_pcb -> status = STATUS_WAITING;
		current_pcb = find_next_live_process();
		current_pcb -> status = STATUS_RUNNING;
		need_sched = FALSE;
	}
}
