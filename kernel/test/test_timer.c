#include "kernel.h"

void test_timer(void) {

	struct Message m;

	while (1)
	{
		m.type = TIMER_SET_ALRM;
		m.int_msg.p1 = current_pcb -> pid;
		m.int_msg.p2 = 1;

		send(TIMER, &m);

		receive(MSG_HARD_INTR, &m);

		if (m.type == TIMER_ALRM_FIRE)
			printk("F");
	}
}
