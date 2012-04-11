#ifndef __CONSOLE_H__
#define __CONSOLE_H__

void init_screen();

void console_printc(char);
void cursor_left(int);
void cursor_right(int);
void save_cursor(void);
void restore_cursor(void);

/* same function as C printf, but works in a different way
   see implementation for more details */
void printk(const char *ctl, ...);

/* system panic */
void panic(const char *ctl, ...);


/* color system panic */
void color_panic(const char *ctl, ...);

#endif
