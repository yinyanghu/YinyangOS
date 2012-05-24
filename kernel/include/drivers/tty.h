#ifndef __TTY_H__
#define __TTY_H__

#define DEV_READ	0xC
#define DEV_WRITE	0xD


#define BACK		14
#define ENTER		28
#define LSHIFT_DOWN	42
#define LSHIFT_UP	170
#define RSHIFT_DOWN	54
#define RSHIFT_UP	182
#define CAPS_ON		58
#define CAPS_OFF	186
#define KEY_UP		72
#define KEY_DOWN	80
#define KEY_LEFT	75
#define KEY_RIGHT	77

#define NR_INPUT_BUF	256
#define NR_OUTPUT_BUF	256
#define NR_INPUT_POOL	1024

#define MODE_CANONIC	1
#define MODE_TERM	2

struct ReadStack {
	pid_t pid;
	void *buf;
	uint_32 length;
	struct ReadStack *next;
};

extern pid_t TTY;

void tty_driver_initialize(void);
void tty_driver_thread(void);
void keyboard_intr(void);


#endif
