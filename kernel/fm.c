#include "kernel.h"


#define Kernel_Size	(1024*1024)
#define File_Size	(64*1024)

pid_t	FM;

static uint_32 do_read_file(uint_32, char *, uint_32, uint_32);

static uint_32 do_read(char *, uint_32, uint_32);

static uint_32 do_write(char *, uint_32, uint_32);

void FileManagement(void) {

	static struct Message m;

	while (TRUE) {
		receive(ANY, &m);
		switch (m.type)
		{
			case FM_READ_FILE:
				m.int_msg.p1 = do_read_file(m.fm_msg.file_name, m.fm_msg.buf, m.fm_msg.offset, m.fm_msg.length);
				m.type = -1;
				send(m.src, &m);
				break;

			case FM_READ:
				m.int_msg.p1 = do_read(m.fm_msg.buf, m.fm_msg.offset, m.fm_msg.length);
				m.type = -1;
				send(m.src, &m);
				break;

			case FM_WRITE:
				m.int_msg.p1 = do_write(m.fm_msg.buf, m.fm_msg.offset, m.fm_msg.length);
				m.type = -1;
				send(m.src, &m);
				break;
		}
	}


}

static uint_32 do_read_file(uint_32 file_name, char *buffer, uint_32 offset, uint_32 length) {

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


static uint_32 do_read(char *buffer, uint_32 offset, uint_32 length) {

	static struct Message m;

	m.type = DEV_READ;
	m.dev_io.pid = current_pcb -> pid;
	m.dev_io.buf = buffer;
	m.dev_io.offset = offset + Kernel_Size;
	m.dev_io.length = length;

	send(IDE, &m);
	receive(IDE, &m);

	return m.int_msg.p1;
}

static uint_32 do_write(char *buffer, uint_32 offset, uint_32 length) {

	static struct Message m;

	m.type = DEV_WRITE;
	m.dev_io.pid = current_pcb -> pid;
	m.dev_io.buf = buffer;
	m.dev_io.offset = offset + Kernel_Size;
	m.dev_io.length = length;

	send(IDE, &m);
	receive(IDE, &m);

	return m.int_msg.p1;
}
