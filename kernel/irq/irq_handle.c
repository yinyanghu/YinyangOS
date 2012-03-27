#include "kernel.h"

#define NR_IRQ_HANDLE 32

/* Structures below is a linked list of function pointers indicating the
   work to be done for each interrupt. Routines will be called one by
   another when interrupt comes.
   For example, the timer interrupts are used for alarm clocks, so
   device driver of timer will use add_irq_handle to attach a simple
   alarm checker. Also, the IDE driver needs to write back dirty cache
   slots periodically. So the IDE driver also calls add_irq_handle
   to register a handler. */

struct IRQ_t {
	void (*routine)(void);
	struct IRQ_t *next;
};

static struct IRQ_t handle_pool[NR_IRQ_HANDLE];
static struct IRQ_t *handles[NR_IRQ];
static int handle_count = 0;

void
add_irq_handle( int irq, void (*func)(void) ) {
	struct IRQ_t *ptr;
	if (handle_count > NR_IRQ_HANDLE) {
		panic("Too many irq registrations!");
	}
	ptr = &handle_pool[handle_count ++]; /* get a free handler */
	ptr->routine = func;
	ptr->next = handles[irq]; /* insert into the linked list */
	handles[irq] = ptr;
}

void schedule();

void irq_handle(struct TrapFrame *tf) {
	int irq = tf->irq;
	current_pcb->esp = tf;

	if (irq < 0) {
		panic("Unhandled exception!");
	}

	if (irq < 1000) {
		panic("Unexpected exception #%d\n", irq);
	} else if (irq >= 1000) {
		int irq_id = irq - 1000;
		struct IRQ_t *f = handles[irq_id];

		while (f != NULL) { /* call handlers one by one */
			f->routine(); 
			f = f->next;
		}
	}

	schedule();
}

