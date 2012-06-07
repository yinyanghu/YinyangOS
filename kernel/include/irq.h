#ifndef __IRQ_H__
#define __IRQ_H__

void init_idt();
void init_intr();
void init_handle();
//void set_need_sched();
void add_irq_handle(int, void (*)(void));

#endif
