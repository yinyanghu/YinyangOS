#include "kernel.h"

uint_32 global_lock_counter = 0;

uint_32 enter_interrupt = 0;

void lock(void)
{
	if (global_lock_counter == 0)
		disable_interrupt();
	++ global_lock_counter;
}


void unlock(void)
{
	-- global_lock_counter;
	if (global_lock_counter == 0 && enter_interrupt == 0)
		enable_interrupt();
}

