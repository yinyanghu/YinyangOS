#include "kernel.h"


pid_t TIMER;

static struct Alarm alarm_pool[NR_PROCESS];
static struct Alarm *alarm_free, *alarm_queue;

static void timer_intr(void);
static void init_100hz_timer(void);
static void init_alarm(void);
static void new_alarm(pid_t, int_32);
static void pop_alarm(void);


void timer_driver_initialize(void) {

	init_100hz_timer();
	init_alarm();
	add_irq_handle(0, timer_intr);
}

void
timer_driver_thread(void) {
#ifdef DEBUG
printk("GOD!\n");
#endif
	static struct Message m;

	while (TRUE) {
#ifdef DEBUG
printk("God want to receive\n");
#endif
		receive(ANY, &m);
#ifdef DEBUG	
printk("Rogar that!\n");
#endif
		switch (m.type) {
			case TIMER_SET_ALRM:
				lock();
				new_alarm(m.int_msg.p1, m.int_msg.p2);
				unlock();
		}
	}
}

static void
timer_intr(void) {
#ifdef DEBUG
printk("Timer Interupt\n");
#endif
	static struct Message m;

	current_pcb->time_elapsed ++;
	if (current_pcb->time_elapsed % NR_PROC_TIME == 0) {
		need_sched = TRUE;
	}
	if (alarm_queue != NULL) {
		alarm_queue->remain_time --;
		while (alarm_queue != NULL && alarm_queue->remain_time <= 0) {
			m.type = TIMER_ALRM_FIRE;
#ifdef DEBUG
printk("God want to send to %d\n", alarm_queue -> pid);
#endif
			send(alarm_queue->pid, &m);
#ifdef DEBUG
printk("after send\n");
#endif


			pop_alarm();
		}
	}
}

static void
init_alarm(void) {
	int i;
	alarm_free = alarm_pool;
	alarm_queue = NULL;
	for (i = 0; i < NR_PROCESS - 1; i ++) {
		alarm_pool[i].next = &alarm_pool[i + 1];
	}
}

static void
init_100hz_timer(void) {
	out_byte(0x43, 0x34);
	out_byte(0x40, 11932 % 256);
	out_byte(0x40, 11932 / 256);
}

static void
new_alarm(pid_t pid, int_32 second) {
	struct Alarm *ptr;
	int time = second * 100;

	ptr = alarm_free;
	alarm_free = alarm_free->next;

	ptr->remain_time = time;
	ptr->pid = pid;

	if (alarm_queue == NULL ||
		ptr->remain_time < alarm_queue->remain_time) {
		ptr->next = alarm_queue;
		alarm_queue = ptr;
	} else {
		struct Alarm *cnt = alarm_queue;
		for (; ; cnt = cnt->next) {
			ptr->remain_time -= cnt->remain_time;
			if (cnt->next == NULL ||
				cnt->next->remain_time > ptr->remain_time) {
				break;
			}
		}
		if (cnt == NULL) panic("No insert position of timer!");
		ptr->next = cnt->next;
		cnt->next = ptr;
	}
	
	if (ptr -> next != NULL)
		ptr -> next -> remain_time -= ptr -> remain_time;
	/*
	for (ptr = ptr->next; ptr != NULL; ptr = ptr->next) {
		ptr->remain_time -= time;
	}
	*/
}

static void
pop_alarm(void) {
#ifdef DEBUG
printk("In Pop_alarm, head = %d\n", alarm_queue -> pid);
#endif
	struct Alarm *ptr;
	ptr = alarm_queue;
	alarm_queue = alarm_queue->next;
	ptr->next = alarm_free;
	alarm_free = ptr;
}
