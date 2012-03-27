#ifndef __IRQ_H__
#define __IRQ_H__

void init_idt();
void init_intr();
void add_irq_handle(int, void (*)(void));

#endif
