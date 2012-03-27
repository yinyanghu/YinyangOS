#ifndef __X86_CPU_H__
#define __X86_CPU_H__

#include "kernel.h"

/* the Control Register 0 */
struct CR0 {
	uint_32 protect_enable      : 1;
	uint_32 monitor_coprocessor : 1;
	uint_32 emulation           : 1;
	uint_32 task_switched       : 1;
	uint_32 extension_type      : 1;
	uint_32 numeric_error       : 1;
	uint_32 pad0                : 10;
	uint_32 write_protect       : 1; 
	uint_32 pad1                : 1; 
	uint_32 alignment_mask      : 1;
	uint_32 pad2                : 10;
	uint_32 no_write_through    : 1;
	uint_32 cache_disable       : 1;
	uint_32 paging              : 1;
};

/* the Control Register 3 (physical address of page directory) */
struct CR3 {
	uint_32 pad0                : 3;
	uint_32 page_write_through  : 1;
	uint_32 page_cache_disable  : 1;
	uint_32 pad1                : 7;
	uint_32 page_directory_base : 20;
};

/* put the CPU into idle, waiting for next interrupt */
static inline void
idle_cpu() {
	asm volatile("hlt");
}

/* read CR0 */
static inline void
load_cr0(struct CR0 *ptr) {
	uint_32 val;
	asm volatile("movl %%cr0, %0" : "=r"(val));
	*((uint_32*)ptr) = val;
}

/* write CR0 */
static inline void
save_cr0(struct CR0 *ptr) {
	uint_32 val = *((uint_32*)ptr);
	asm volatile("movl %0, %%cr0" : : "r"(val));
}

/* write CR3, notice that CR3 are never read in Nanos */
static inline void
save_cr3(struct CR3 *ptr) {
	uint_32 val = *((uint_32*)ptr);
	asm volatile("movl %0, %%cr3" : : "r"(val));
}

/* modify the value of GDTR */
static inline void
save_gdt(void *addr, uint_32 size) {
	static volatile uint_16 data[3];
	data[0] = size - 1;
	data[1] = (uint_32)addr;
	data[2] = ((uint_32)addr) >> 16;
	asm volatile("lgdt (%0)" : : "r"(data));
}

/* modify the value of IDTR */
static inline void
save_idt(void *addr, uint_32 size) {
	static volatile uint_16 data[3];
	data[0] = size - 1;
	data[1] = (uint_32)addr;
	data[2] = ((uint_32)addr) >> 16;
	asm volatile("lidt (%0)" : : "r"(data));
}

static inline void
enable_interrupt(void) {
	asm volatile("sti");
}

static inline void
disable_interrupt(void) {
	asm volatile("cli");
}


#define NR_IRQ    256

#endif
