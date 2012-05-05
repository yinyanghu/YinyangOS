#include "kernel.h"

#define BACK          14
#define ENTER         28
#define LSHIFT_DOWN   42
#define LSHIFT_UP     170
#define RSHIFT_DOWN   54
#define RSHIFT_UP     182
#define CAPS_ON       58
#define CAPS_OFF      186
#define KEY_UP        72
#define KEY_DOWN      80
#define KEY_LEFT      75
#define KEY_RIGHT     77

#define NR_INPUT_BUF  256
#define NR_OUTPUT_BUF 256
#define NR_INPUT_POOL 1024

#define MODE_CANONIC    1
#define MODE_TERM     2

pid_t TTY;

void keyboard_intr(void);
static char get_ascii(int);
static void canonic_mode_getch(uint_32);
static void canonic_mode_write(struct PCB *, void *, uint_32);
static void canonic_wakeup_read(void);

static int key_maps[2][256];
static int shift_level = 0;
static boolean caps_on = FALSE;
static int mode = MODE_CANONIC;
static boolean dirty = FALSE;

static char input_buffer[NR_INPUT_BUF];
static char output_buffer[NR_OUTPUT_BUF];
static int  input_length = 0;
static int  cursor_pos = 0;

static char input_pool[NR_INPUT_POOL];
static int  f = 0, r = 0;

struct ReadStack {
	pid_t pid;
	void *buf;
	uint_32 length;
	struct ReadStack *next;
};
static struct ReadStack read_pool[NR_PROCESS];
static struct ReadStack *free, *top;
static void init_read_stack(void);
static void read_stack_push(pid_t, void *, uint_32);

void
tty_driver_thread(void) {
	static struct Message m;

	add_irq_handle(1, keyboard_intr);
	init_read_stack();

	while (TRUE) {
		receive(ANY, &m);
		if (m.src == MSG_HARD_INTR) {
			uint_32 code = in_byte(0x60);
			uint_32 val  = in_byte(0x61);
			out_byte(0x61, val | 0x80);
			out_byte(0x61, val);

			if (mode == MODE_CANONIC) {
				canonic_mode_getch(code);
			}
		} else {
			switch (m.type) {
				case DEV_READ:
					read_stack_push(m.dev_io.pid, m.dev_io.buf, m.dev_io.length);
					canonic_wakeup_read();
					break;
				case DEV_WRITE:
					if (mode == MODE_CANONIC) {
						struct PCB *pcb = fetch_pcb(m.dev_io.pid);
						canonic_mode_write(pcb, m.dev_io.buf, m.dev_io.length);
					}
					break;
			}
		}
	}
}

void
keyboard_intr(void) {
	static struct Message m;
	send(TTY, &m);
}

static int key_maps[2][256] = {
	{   0 ,   0 ,  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',
	   '9',  '0',  '-',  '=',   0 , '\t',  'q',  'w',  'e',  'r',
	   't',  'y',  'u',  'i',  'o',  'p',  '[',  ']',   0,    0 ,
	   'a',  's',  'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',
	  '\'',  '`',   0 , '\\',  'z',  'x',  'c',  'v',  'b',  'n',
	   'm',  ',',  '.',  '/',   0 ,   0 ,   0 ,  ' '},
	{   0 ,   0 ,  '!',  '@',  '#',  '$',  '%',  '^',  '&',  '*',
	   '(',  ')',  '_',  '+',   0 ,   0 ,  'Q',  'W',  'E',  'R',
	   'T',  'Y',  'U',  'I',  'O',  'P',  '{',  '}',   0,    0 ,
	   'A',  'S',  'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',
	   '"',  '~',   0 ,  '|',  'Z',  'X',  'C',  'V',  'B',  'N',
	   'M',  '<',  '>',  '?',   0 ,   0 ,   0 ,  ' '},
};

static char
get_ascii(int code) {
	int index;

	switch (code) {
		case LSHIFT_DOWN: case RSHIFT_DOWN:
			shift_level ++;
			return 0;
		case LSHIFT_UP: case RSHIFT_UP:
			shift_level --;
			return 0;
		case CAPS_ON:
			caps_on = TRUE;
			return 0;
		case CAPS_OFF:
			caps_on = FALSE;
			return 0;
	}

	index = shift_level > 0;
	if ( key_maps[0][code] >= 'a' && key_maps[0][code] <= 'z') {
		index ^= caps_on;
	}
	return key_maps[index][code];
}

static void
update_input(void) {
	int i;
	if (dirty == TRUE) {
		dirty = FALSE;
		console_printc('\n');
		for (i = 0; i < cursor_pos; i ++) {
			console_printc(input_buffer[i]);
		}
		for (i = cursor_pos; i < input_length; i ++) {
			console_printc(input_buffer[i]);
		}
		cursor_left(input_length - cursor_pos);
	}
}

static void
canonic_mode_getch(uint_32 code) {
	char ch = get_ascii(code);
	int i;

	update_input();
	if (ch != 0) {
		console_printc(ch);
		for (i = input_length; i > cursor_pos; i --) {
			input_buffer[i] = input_buffer[i - 1];
		}
		input_buffer[++ input_length] = 0;
		input_buffer[cursor_pos ++] = ch;
		for (i = cursor_pos; i < input_length; i ++) {
			console_printc(input_buffer[i]);
		}
		cursor_left(input_length - cursor_pos);
	} else {
		switch (code) {
			case BACK:
				if (cursor_pos > 0) {
					for (i = cursor_pos - 1; i < input_length; i ++) {
						input_buffer[i] = input_buffer[i + 1];
					}
					input_length --;
					cursor_pos --;
					cursor_left(1);
					for (i = cursor_pos; i < input_length; i ++) {
						console_printc(input_buffer[i]);
					}
					console_printc(' ');
					cursor_left(input_length - cursor_pos + 1);
				}
				break;
			case KEY_LEFT:
				if (cursor_pos > 0) {
					cursor_pos --;
					cursor_left(1);
				}
				break;
			case KEY_RIGHT:
				if (cursor_pos < input_length) {
					cursor_pos ++;
					cursor_right(1);
				}
				break;
			case ENTER:
				input_buffer[input_length] = '\n';
				input_buffer[++ input_length] = 0;
				for (i = 0; i <= input_length; i ++) {
					input_pool[r ++] = input_buffer[i];
					r %= NR_INPUT_POOL;
				}
				canonic_wakeup_read();
				console_printc('\n');
				input_buffer[cursor_pos = 0] = 0;
				input_length = 0;
				break;
		}
	}
}

static void
canonic_mode_write(struct PCB *pcb, void *buf, uint_32 length) {
	uint_32 i, j, count;
	static struct Message m;

	for (i = 0; i < length; i += NR_OUTPUT_BUF) {
		count = (length - i >= NR_OUTPUT_BUF) ? NR_OUTPUT_BUF : length - i;
		copy_to_kernel(pcb, output_buffer, buf + i, count);
		for (j = 0; j < count; j ++) {
			console_printc( ((char*)output_buffer)[j] );
		}
	}
	if (input_length != 0) {
		dirty = TRUE;
	}

	m.int_msg.p1 = length;
	send(pcb->pid, &m);
}

static void
init_read_stack(void) {
	int i;
	for (i = 0; i < NR_PROCESS - 1; i ++) {
		read_pool[i].next = &read_pool[i + 1];
	}
	read_pool[i].next = NULL;
	free = read_pool;
	top = NULL;
}

static void
read_stack_push(pid_t pid, void *buf, uint_32 length) {
	struct ReadStack *ptr = free;
	free = free->next;
	ptr->next = top;
	top = ptr;
	top->pid = pid;
	top->buf = buf;
	top->length = length;
}
static void
read_stack_pop(struct ReadStack *ptr) {
	top = top->next;
	ptr->next = free;
	free = ptr;
}

static void
canonic_wakeup_read(void) {
	int i, nread = 0;
	static struct Message m;

	if (top != NULL && f != r) {
		for (i = 0; i < top->length && input_pool[f] != 0; i ++) {
			copy_from_kernel(fetch_pcb(top->pid),
					top->buf + i, input_pool + f, 1);
			f = (f + 1) % NR_INPUT_POOL;
			nread ++;
		}
		if (input_pool[f] == 0) {
			f = (f + 1) % NR_INPUT_POOL;
		}
		m.int_msg.p1 = nread;
		send(top->pid, &m);
		read_stack_pop(top);
	}
}
