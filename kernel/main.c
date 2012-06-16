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
	
	/* Initialize the services. */
	init_service();

	/* Initialize process table. You should fill this. */
	init_proc();

	/* Load drivers and servers. */
	Create_kthread(timer_driver_thread);
	TIMER = 1;

	Create_kthread(tty_driver_thread);
	TTY = 2;

	Create_kthread(ide_driver_thread);
	IDE = 3;

	Create_kthread(FileManagement);
	FM = 4;

	Create_kthread(MemoryManagement);
	MM = 5;

	Create_kthread(ProcessManagement);
	PM = 6;

	
	welcome();

	//uint_32 stack_ptr;
	//asm volatile("pushl %esp");
	//asm volatile("movl %%esp, %0" : "=r"(stack_ptr));
	//printk("%d\n", stack_ptr);
	//printk("%d\n", *(uint_32 *)(stack_ptr + 4));
	//panic("sss");
	//current_pcb -> esp = (void *)(stack_ptr + 4);
	asm volatile("addl $0xC0000000, %esp");

	enable_interrupt();

	/* load init user process */
	Create_uthread(1);
	//Create_uthread(2);


	/* This context now becomes the idle proess. */
	while (1) {
		idle_cpu();
		
	}
}

void init_service(void) {
	init_MM();
	init_PM();
}


void load_info(void) {
	int_32	i;
	for (i = 0; i < 8; ++ i)
		printk("Process %d, CR3 = %x\n", i, *((uint_32 *)(&((Proc + i) -> cr3))));
	printk("============================\n");
}


void welcome(void) {
	color_printk("Welcome to Yinyanghu OS!\n");
	color_printk("Hello Yinyanghu!\n");
	/*
	uint_32 i = 0;
	for (i = 0; i < 256; ++ i)
		color_console_printc(i, BLACK, 'Y');
	panic("Find Color!\n");
	*/
	/*
	static char prompt[] = "Hello, OS World!\n";
	const char *str;
	for (str = prompt; *str; str ++)
		console_printc(*str);
	*/
}
