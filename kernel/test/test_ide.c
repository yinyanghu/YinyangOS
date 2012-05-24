#include "kernel.h"

void test_ide(void) {
	static struct Message m;
	
	char ch[256];

	int i, j;
	while (TRUE) {

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
}


