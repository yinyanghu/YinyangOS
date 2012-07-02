#include "oslib.h"


/* System Call */
int read(char *buf, int length) {
	return syscall_2(INT80_READ, (int)buf, length);
}


int write(char *buf, int length) {
	return syscall_2(INT80_WRITE, (int)buf, length);
}

int fork(void) {
	return syscall_0(INT80_FORK);	
}

int waitpid(int pid) {
	return syscall_1(INT80_WAITPID, pid);
}

int getpid(void) {
	return syscall_0(INT80_GETPID);
}

void exit(int exit_code) {
	syscall_1(INT80_EXIT, exit_code);
}

void sleep(int time) {
	syscall_1(INT80_SLEEP, time);
}


int execv(int file_name, char *argv[]) {

	static char argv_buf[Buffer_Size];

	int argv_buf_ptr = 0;	

	char *str;

	char **ptr = argv;

	while (*ptr != 0)
	{
		str = *ptr;
		while (*str != 0)
		{
			argv_buf[argv_buf_ptr ++] = *str;
			++ str;
		}

		argv_buf[argv_buf_ptr ++] = 0;
		++ ptr;
	}

	argv_buf[argv_buf_ptr] = 0;

	//argv[0] is program name

	//printf("why?\n");
	return syscall_2(INT80_EXEC, file_name, (int)argv_buf);
}


/* Library function */
int strlen(char *str) {

	int len = 0;
	for (; *str != 0; ++ str, ++ len);
	return len;
}


void scanf(char *ctl, ...) {
	void **args = (void **)&ctl + 1;

	char	*ptr;
	int		len;

	for (; *ctl != 0; ++ ctl)
	{
		if (*ctl == '%')
		{
			switch (*(++ ctl))
			{
				case 's':	ptr = (char *)(*args ++);
							len = read(ptr, Buffer_Size - 1);
							*(ptr + len) = 0;
							break;
			}
		}
	}
}

void printf(char *ctl, ...) {
	void **args = (void **)&ctl + 1;

	static char buf[Buffer_Size];
	int buf_ptr;

	int temp[12];
	int dec;
	unsigned int hex;
	char *ch;
	int i;

	buf_ptr = 0;
	
	for (; *ctl != 0; ++ ctl)
	{
		if (*ctl != '%') 
		{
			buf[buf_ptr ++] = *ctl;
		}
		else
		{
			switch (*(++ ctl))
			{
				case 'd':	dec = *((int *)args ++);
							if (dec == 0)
								buf[buf_ptr ++] = '0';
							else
							{
								if (dec < 0)
								{
									buf[buf_ptr ++] = '-';
									dec = -dec;
								}
								for (i = 0; dec != 0; dec /= 10, ++ i)
									temp[i] = dec % 10;
								for (-- i; i >= 0; -- i)
									buf[buf_ptr ++] = (char)(temp[i] + 48);
							}
							break;

				case 'x':	hex = *((unsigned int *)args ++);
							if (hex == 0)
								buf[buf_ptr ++] = '0';
							else
							{
								for (i = 0; hex != 0; hex = hex >> 4, ++ i)
									temp[i] = hex & 0xF;
								for (-- i; i >= 0; -- i)
									if (temp[i] < 10)
										buf[buf_ptr ++] = (char)(temp[i] + 48);
									else
										buf[buf_ptr ++] = (char)(temp[i] + 55);
							}
							break;

				case '%':	buf[buf_ptr ++] = '%';
							break;

				case 'c':	buf[buf_ptr ++] = *((char *)args ++);
							break;

				case 's':	for (ch = (char *)(*args ++); *ch != 0; ++ ch)
							buf[buf_ptr ++] = *ch;
							break; 
			}
		}

	}
	write(buf, buf_ptr);
}

