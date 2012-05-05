#ifndef __MY_PROC__
#define __MY_PROC__


void process_A(void);
void process_B(void);
void process_C(void);

void init_producer_consumer(void);
void producer(void);
void consumer(void);

extern struct Sem_type empty;
extern struct Sem_type full;
extern struct Sem_type mutex;
extern int PC_counter;


#endif
