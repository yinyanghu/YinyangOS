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
