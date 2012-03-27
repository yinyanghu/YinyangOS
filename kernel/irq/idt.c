#include "kernel.h"

#define INTERRUPT_GATE_32   0xE
#define TRAP_GATE_32        0xF

/* the global IDT list
   in Nanos, each entry of the IDT is either an interrupt gate, or a trap gate */
struct GateDescriptor idt[NR_IRQ];

/* setup a interrupt gate for interrupt handlers */
void set_intr(struct GateDescriptor *ptr, uint_32 selector, uint_32 offset, uint_32 dpl) {
	ptr->offset_15_0 = offset & 0xFFFF;
	ptr->segment = selector;
	ptr->pad0 = 0;
	ptr->type = INTERRUPT_GATE_32;
	ptr->system = FALSE;
	ptr->privilege_level = dpl;
	ptr->present = TRUE;
	ptr->offset_31_16 = (offset >> 16) & 0xFFFF;
}

/* setup a trap gate for cpu exceptions */
void set_trap(struct GateDescriptor *ptr, uint_32 selector, uint_32 offset, uint_32 dpl) {
	ptr->offset_15_0 = offset & 0xFFFF;
	ptr->segment = selector;
	ptr->pad0 = 0;
	ptr->type = TRAP_GATE_32;
	ptr->system = FALSE;
	ptr->privilege_level = dpl;
	ptr->present = TRUE;
	ptr->offset_31_16 = (offset >> 16) & 0xFFFF;
}

void irq0();
void irq1();
void irq14();
void vec0();
void vec1();
void vec2();
void vec3();
void vec4();
void vec5();
void vec6();
void vec7();
void vec8();
void vec9();
void vec10();
void vec11();
void vec12();
void vec13();
void vec14();
void vecsys();

void irq_empty();

void init_idt() {
	int i;
	for (i = 0; i < NR_IRQ; i ++) {
		set_trap(idt + i, SEG_KERNEL_CODE << 3, (uint_32)irq_empty, DPL_KERNEL);
	}
	set_trap(idt + 0, SEG_KERNEL_CODE << 3, (uint_32)vec0, DPL_KERNEL);
	set_trap(idt + 1, SEG_KERNEL_CODE << 3, (uint_32)vec1, DPL_KERNEL);
	set_trap(idt + 2, SEG_KERNEL_CODE << 3, (uint_32)vec2, DPL_KERNEL);
	set_trap(idt + 3, SEG_KERNEL_CODE << 3, (uint_32)vec3, DPL_KERNEL);
	set_trap(idt + 4, SEG_KERNEL_CODE << 3, (uint_32)vec4, DPL_KERNEL);
	set_trap(idt + 5, SEG_KERNEL_CODE << 3, (uint_32)vec5, DPL_KERNEL);
	set_trap(idt + 6, SEG_KERNEL_CODE << 3, (uint_32)vec6, DPL_KERNEL);
	set_trap(idt + 7, SEG_KERNEL_CODE << 3, (uint_32)vec7, DPL_KERNEL);
	set_trap(idt + 8, SEG_KERNEL_CODE << 3, (uint_32)vec8, DPL_KERNEL);
	set_trap(idt + 9, SEG_KERNEL_CODE << 3, (uint_32)vec9, DPL_KERNEL);
	set_trap(idt + 10, SEG_KERNEL_CODE << 3, (uint_32)vec10, DPL_KERNEL);
	set_trap(idt + 11, SEG_KERNEL_CODE << 3, (uint_32)vec11, DPL_KERNEL);
	set_trap(idt + 12, SEG_KERNEL_CODE << 3, (uint_32)vec12, DPL_KERNEL);
	set_trap(idt + 13, SEG_KERNEL_CODE << 3, (uint_32)vec13, DPL_KERNEL);
	set_trap(idt + 14, SEG_KERNEL_CODE << 3, (uint_32)vec14, DPL_KERNEL);

	/* the system call 0x80 */
	set_trap(idt + 0x80, SEG_KERNEL_CODE << 3, (uint_32)vecsys, DPL_KERNEL);

	set_intr(idt+32 + 0, SEG_KERNEL_CODE << 3, (uint_32)irq0, DPL_KERNEL);
	set_intr(idt+32 + 1, SEG_KERNEL_CODE << 3, (uint_32)irq1, DPL_KERNEL);
	set_intr(idt+32 + 14, SEG_KERNEL_CODE << 3, (uint_32)irq14, DPL_KERNEL);

	/* the ``idt'' is its virtual address */
	save_idt(idt, sizeof(idt));
}

