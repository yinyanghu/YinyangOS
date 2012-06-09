#ifndef __IRQ_H__
#define __IRQ_H__

#define INT80_SCHEDULE	0x0
#define INT80_READ	0x1
#define INT80_WRITE	0x2

void init_idt();
void init_intr();
void init_handle();
//void set_need_sched();
void add_irq_handle(int, void (*)(void));

#endif
