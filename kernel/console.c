#include "kernel.h"

/* I/O ports for 6845 video controller */
#define CRT_PORT_SEL 0x3D4
#define CRT_PORT_DAT 0x3D5

/* the console has 25 rows, 80 characters each */
#define CRT_COLUMN   80
#define CRT_ROW      25

/* color codes */
#define BLACK		0
#define BLUE		1
#define WHITE		7
#define RED			12


/* The memory-map of character screen is located at 0xB8000.
   This is a physical address which the kernel should not directly
   access. */
static uint_16 *vmem = pa_to_va( (uint_16*)0xB8000 );
static int x = 0, y = 0;

/* Set the cursor position. The I/O registers are 8-bit,
   so the hardware separates the 16-bit cursor position
   into two registers. */
static void move_cursor(int x, int y) {
	out_byte(CRT_PORT_SEL, 0x0E);
	out_byte(CRT_PORT_DAT, ((x * 80 + y) >> 8) & 0xFF );
	out_byte(CRT_PORT_SEL, 0x0F);
	out_byte(CRT_PORT_DAT, ((x * 80 + y)     ) & 0xFF );
}

void init_screen(void) {
	int i;
	for (i = 0; i < CRT_COLUMN * CRT_ROW; i ++)
		vmem[i] = (BLACK << 12) | (WHITE << 8) | ' ';
	x = 0; y = 0;
	move_cursor(0, 0);
}

void cursor_left(int count) {
	while (count --) {
		y --;
		if (y < 0) {
			y = CRT_COLUMN - 1;
			x --;
		}
		move_cursor(x, y);
	}
}

void cursor_right(int count) {
	while (count --) {
		y ++;
		if (y >= CRT_COLUMN) {
			x ++;
			y = 0;
		}
	}
	move_cursor(x, y);
}


static void wrap_screen(void) {
	int i;
	for (i = CRT_COLUMN; i < CRT_ROW * CRT_COLUMN; i ++)
		vmem[i - CRT_COLUMN] = vmem[i];
	for (i = 0; i < CRT_COLUMN; i ++)
		vmem[CRT_ROW * CRT_COLUMN - i - 1] = (BLACK << 12) | (WHITE << 8) | ' ';
}

/* find the postion, then put the character */
void color_console_printc(int forecolor, int backcolor, char ch) {
	switch (ch) {
	case '\n': /* everyone will get mad if linebreak is not provided */
		x ++;
		y = 0;
		break;
	default: /* print the character otherwise */
		vmem[x * CRT_COLUMN + y] = (backcolor << 12) | (forecolor << 8) | ch;
		y ++;
		if (y >= CRT_COLUMN) {
			x ++;
			y = 0;
		}
	}
	if (x >= CRT_ROW) { /* bottom touched! */
		x = CRT_ROW - 1;
		wrap_screen();
	}
	move_cursor(x, y);
}

void console_printc(char ch) {
	color_console_printc(WHITE, BLACK, ch);
}



void red_console_printc(char ch) {
	color_console_printc(RED, BLACK, ch);
}


void printk(const char *ctl, ...) {
	void **args = (void **)&ctl + 1;
	vfprintf(console_printc, ctl, args);
}

void color_printk(const char *ctl, ...) {
	void **args = (void **)&ctl + 1;
	vfprintf(red_console_printc, ctl, args);
}

void panic(const char *ctl, ...) {
	void *args = (void **)&ctl + 1;
	vfprintf(red_console_printc, ctl, args);
	disable_interrupt();
	while (1) {
		idle_cpu();
	}
}

void white_black_panic(const char *ctl, ...) {
	void *args = (void **)&ctl + 1;
	vfprintf(console_printc, ctl, args);
	disable_interrupt();
	while (1) {
		idle_cpu();
	}
}

