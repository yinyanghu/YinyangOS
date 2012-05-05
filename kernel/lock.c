#include "kernel.h"

int global_lock_counter = 0;

boolean enter_interrupt = FALSE;

void lock(void)
{
	if (global_lock_counter == 0)
		disable_interrupt();
	++ global_lock_counter;
}


void unlock(void)
{
	-- global_lock_counter;
	if (global_lock_counter == 0 && !enter_interrupt)
		enable_interrupt();
}

