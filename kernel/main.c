#include "kernel.h"


void welcome(void);
void init_service(void);

void
os_init(void) {
	/* Notice that when we are here, IF is always 0 (see bootloader) */

	/* We must set up kernel virtual memory first because our kernel
	   thinks it is located in 0xC0000000.
	   Before setting up correct paging, no global variable can be used. */
	init_page();

	/* Reset the GDT. Although user processes in Nanos run in Ring 0,
	   they have their own virtual address space. Therefore, the
	   old GDT located in physical address 0x7C00 cannot be used again. */
	init_segment();

	/* Flush the screen, move cursor to (0, 0). */
	init_screen();

	/* Initialize interrupt handlers */
	init_handle();

	/* Set up interrupt and exception handlers,
	   just as we did in the game. */
	init_idt();

	/* Initialize the intel 8259 PIC. */
	init_intr();

	/* Initialize the drivers. */
	init_drivers();

	/* Initialize process table. You should fill this. */
	init_proc();

	/* Initialize the services. */
	init_service();
	
	welcome();

	enable_interrupt();

	/* This context now becomes the idle proess. */
	while (1) {
		idle_cpu();
	}
}

void init_service(void) {
	init_mm();
}

void
welcome(void) {
	static char prompt[] = "Hello, OS World!\n";
	const char *str;
	for (str = prompt; *str; str ++)
		console_printc(*str);
	printk("Hello Yinyanghu!\n");
}
