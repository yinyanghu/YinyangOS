#include "kernel.h"

/*
void test_ide(void) {

	int *p;
	while (1)
	{
		p = &IDE;
		printk("First %d\n", *p);
		p -= KOFFSET;
		printk("Second %d\n", *p);
	}

}

*/

void test_ide(void) {
	static struct Message m;
	
	char ch[256];

	int i, j;
	//int total = 1000;
	while (1) {

		for (i = 0; i < 10; ++ i)
			for (j = 1; j <= 3; ++ j)
			{
				m.type = FM_READ;
				m.fm_msg.file_name = j;
				m.fm_msg.buf = ch;
				m.fm_msg.offset = i;
				m.fm_msg.length = 1;

				send(FM, &m);
				receive(FM, &m);

				printk("%s\n", ch);
			}

	}

	/*
	int total_time = 0;
	for (i = 0; i <= 5; ++ i)
		total_time += Proc[i].time_elapsed;
	
	
	printk("Finish!\n%d\n", total_time);
	panic("gg");
	*/
}

