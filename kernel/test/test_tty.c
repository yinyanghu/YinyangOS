#include "kernel.h"

#define Buffer_Size	128

void test_tty(void) {

	struct Message m;
	char buf[Buffer_Size];
	int i;
	
	while (1) {

		m.type = DEV_READ;
		m.dev_io.pid = current_pcb -> pid;
		m.dev_io.length = Buffer_Size;// - 1;
		m.dev_io.buf = buf;
		

		send(TTY, &m);
		receive(TTY, &m);

		for (i = 0; i < m.int_msg.p1; ++ i)
			if ('a' <= buf[i] && buf[i] <= 'z')
				buf[i] = buf[i] - 'a' + 'A';

		m.type = DEV_WRITE;
		m.dev_io.pid = current_pcb -> pid;
		m.dev_io.length = i;
		m.dev_io.buf = buf;

		send(TTY, &m);
		receive(TTY, &m);
	}
}


