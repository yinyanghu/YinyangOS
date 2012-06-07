#ifndef __LOCK_H__
#define __LOCK_H__

extern uint_32 global_lock_counter;
extern uint_32 enter_interrupt;


void lock(void);
void unlock(void);

#endif

