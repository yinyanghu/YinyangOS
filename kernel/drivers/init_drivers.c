#include "kernel.h"

void init_drivers(void) {

	timer_driver_initialize();
	tty_driver_initialize();
	ide_driver_initialize();
}
