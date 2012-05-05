#ifndef __LOCK_H__
#define __LOCK_H__

extern int global_lock_counter;
extern boolean enter_interrupt;


void lock(void);
void unlock(void);

#endif

