#ifndef	__SEMAPHORE_H__
#define __SEMAPHORE_H__

#define Max_Sem		50
#define	Max_Sem_Queue	50

struct Sem_type {
	int	token;
	int	queue_tail;
	pid_t	queue[Max_Sem_Queue];
};


void Sem_init(struct Sem_type *sem, int init_token);
//struct Sem_type* Sem_new(void);
void Sem_P(struct Sem_type *sem);
void Sem_V(struct Sem_type *sem);

#endif
