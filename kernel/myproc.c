#include "kernel.h"

void process_A(void) {
	while (1)
		printk("A");
}


void process_B(void) {
	while (1)
		printk("B");
}

void process_C(void) {
	while (1)
		printk("C");
}




//**********************************************************************


struct Sem_type empty;
struct Sem_type full;
struct Sem_type mutex;
int	PC_counter;

void init_producer_consumer(void) {
	Sem_init(&empty, 20);
	Sem_init(&full, 0);
	Sem_init(&mutex, 1);
	PC_counter = 0;
}
	


void producer(void) {

	//panic("sb");
	while (1)
	{
		Sem_P(&empty);
		Sem_P(&mutex);
		++ PC_counter;
		printk("producing! %d\n", PC_counter);
		Sem_V(&mutex);
		Sem_V(&full);
	}
}

void consumer(void) {

//	panic("fuck");
	while (1)
	{
		Sem_P(&full);
		Sem_P(&mutex);
		-- PC_counter;
		printk("consuming! %d\n", PC_counter);
		Sem_V(&mutex);
		Sem_V(&empty);
	}
}


//**********************************************************************
