#include "kernel.h"


#define Kernel_Size	(1024*1024)
#define File_Size	(64*1024)

pid_t	FM;

static uint_32 do_read(uint_32 file_name, char *buffer, uint_32 offset, uint_32 length);

void FileManagement(void) {

	static struct Message m;
	static uint_32 read_size;

	while (TRUE) {
		receive(ANY, &m);
		if (m.type == FM_READ)
		{
			read_size = do_read(m.fm_msg.file_name, m.fm_msg.buf, m.fm_msg.offset, m.fm_msg.length);
	//	panic("notify!\n");
			m.type = -1;
			m.int_msg.p1 = read_size;
			send(m.src, &m);
			printk("send to %d\n", m.src);
		}
	}


}

static uint_32 do_read(uint_32 file_name, char *buffer, uint_32 offset, uint_32 length) {

	static struct Message m;

	m.type = DEV_READ;
	m.dev_io.pid = current_pcb -> pid;
	m.dev_io.buf = buffer;
	m.dev_io.offset = offset + Kernel_Size + (file_name - 1) * File_Size;
	m.dev_io.length = length;

	send(IDE, &m);
	receive(IDE, &m);

	return m.int_msg.p1;
}

