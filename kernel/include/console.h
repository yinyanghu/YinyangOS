#ifndef __CONSOLE_H__
#define __CONSOLE_H__

void init_screen(void);

void console_printc(char);
void cursor_left(int);
void cursor_right(int);

void color_console_printc(int, int, char);
void console_printc(char);
void red_console_printc(char);
void green_console_printc(char);

/* same function as C printf, but works in a different way
   see implementation for more details */
void printk(const char *, ...);
void red_printk(const char *, ...);
void green_printk(const char *, ...);

/* color system panic */
/* system panic */
void panic(const char *, ...);
void white_black_panic(const char *, ...);


#endif
