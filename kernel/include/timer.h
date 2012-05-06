#ifndef __TIMER_H__
#define __TIMER_H__


#define NR_PROC_TIME	5
#define NR_PROCESS	32

struct Alarm {
	pid_t pid;
	int_32 remain_time;
	struct Alarm *next;
};

extern pid_t TIMER;

void timer_driver_thread(void);

#endif

