#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "kernel.h"

#define KOFFSET 0xC0000000

void make_invalid_pde(struct PageDirectoryEntry *);
void make_invalid_pte(struct PageTableEntry *);
void make_pde(struct PageDirectoryEntry *, void *);
void make_pte(struct PageTableEntry *, void *);

extern struct PageDirectoryEntry kpagedir[];

void init_page(void);
void init_segment(void);

#define va_to_pa(addr) \
	((void*)(((uint_32)(addr)) - KOFFSET))

#define pa_to_va(addr) \
	((void*)(((uint_32)(addr)) + KOFFSET))

#define USER_MEM_LOW    (16 * 1024 * 1024)
#define USER_MEM_HIGH   (128 * 1024 * 1024)

static inline boolean
in_kernel(void *addr) {
	return (uint_32)addr >= KOFFSET;
}

#endif
