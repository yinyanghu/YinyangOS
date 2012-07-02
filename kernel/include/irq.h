#ifndef __IRQ_H__
#define __IRQ_H__

#define INT80_SCHEDULE		0x0
#define INT80_READ			0x1
#define INT80_WRITE			0x2
#define INT80_SLEEP			0x3
#define INT80_EXIT			0x4
#define INT80_GETPID		0x5
#define INT80_WAITPID		0x6
#define INT80_FORK			0x7
#define INT80_EXEC			0x8

void init_idt(void);
void init_intr(void);
void init_handle(void);
//void set_need_sched();
void add_irq_handle(int, void (*)(void));

#endif
