#include "kernel.h"

void
make_invalid_pde(struct PageDirectoryEntry *p) {
	*(uint_32*)p = 0;
}
void
make_invalid_pte(struct PageTableEntry *p) {
	*(uint_32*)p = 0;
}

void
make_pde(struct PageDirectoryEntry *p, void *addr) {
	*(uint_32*)p = 0;
	p->page_frame = ((uint_32)addr) >> 12;
	p->present = 1;
	p->read_write = 1;
	p->user_supervisor = 1;
}

void
make_pte(struct PageTableEntry *p, void *addr) {
	*(uint_32*)p = 0;
	p->page_frame = ((uint_32)addr) >> 12;
	p->present = 1;
	p->read_write = 1;
	p->user_supervisor = 1;
}

