#include "kernel.h"

void test_timer_f(void) {

	struct Message m;

	while (1)
	{
		m.type = TIMER_SET_ALRM;
		m.int_msg.p1 = current_pcb -> pid;
		m.int_msg.p2 = 4;

		send(TIMER, &m);

		receive(MSG_HARD_INTR, &m);

		if (m.type == TIMER_ALRM_FIRE)
			printk("F");

		send(4, &m);

	}
}


void test_timer_u(void) {

	struct Message m;

	while (1)
	{
		receive(3, &m);

		m.type = TIMER_SET_ALRM;
		m.int_msg.p1 = current_pcb -> pid;
		m.int_msg.p2 = 1;

		send(TIMER, &m);

		receive(MSG_HARD_INTR, &m);

		if (m.type == TIMER_ALRM_FIRE)
			printk("U");

		send(5, &m);

	}
}



void test_timer_c(void) {

	struct Message m;

	while (1)
	{
		receive(4, &m);

		m.type = TIMER_SET_ALRM;
		m.int_msg.p1 = current_pcb -> pid;
		m.int_msg.p2 = 1;

		send(TIMER, &m);

		receive(MSG_HARD_INTR, &m);

		if (m.type == TIMER_ALRM_FIRE)
			printk("C");

		send(6, &m);

	}
}


void test_timer_k(void) {

	struct Message m;

	while (1)
	{
		receive(5, &m);

		m.type = TIMER_SET_ALRM;
		m.int_msg.p1 = current_pcb -> pid;
		m.int_msg.p2 = 1;

		send(TIMER, &m);

		receive(MSG_HARD_INTR, &m);

		if (m.type == TIMER_ALRM_FIRE)
			printk("K");

	}
}


