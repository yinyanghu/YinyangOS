#include "kernel.h"

struct PageDirectoryEntry kpagedir[NR_PDE_ENTRY] align_to_page;
struct PageTableEntry kpagetable[USER_MEM_HIGH / PAGE_SIZE] align_to_page;

/* Build a page table for the kernel */
void
init_page(void) {
	struct PageDirectoryEntry *pdir;
	struct PageTableEntry *pent;
	struct CR0 cr0;
	struct CR3 cr3;
	uint_32 i, j;

	pdir = (struct PageDirectoryEntry *)va_to_pa(kpagedir);
	pent = (struct PageTableEntry *)va_to_pa(kpagetable);

	for (i = 0; i < NR_PDE_ENTRY; i ++) {
		make_invalid_pde(&pdir[i]);
	}

	for (i = 0; i < USER_MEM_HIGH / PD_SIZE; i ++) {
		make_pde(&pdir[i], pent);
		make_pde(&pdir[i + KOFFSET / PD_SIZE], pent);
		for (j = 0; j < NR_PTE_ENTRY; j ++) {
			make_pte(pent, (void*)((i * NR_PTE_ENTRY + j) << 12));
			pent ++;
		}
	}

	/* make CR3 to be the entry of page directory */
	*(uint_32*)&cr3 = 0;
	cr3.page_directory_base = ((uint_32)pdir) >> 12;
	save_cr3(&cr3);

	/* set PG bit in CR0 */
	load_cr0(&cr0);
	cr0.paging = 1;
	save_cr0(&cr0);
}

/* GDT in the kernel's memory, whoes virtual memory is greater than
   0xC0000000. */
static struct SegmentDescriptor gdt[NR_SEGMENTS];

static void
set_segment(struct SegmentDescriptor *ptr, uint_32 type) {
	ptr->limit_15_0  = 0xFFFF;
	ptr->base_15_0   = 0x0;
	ptr->base_23_16  = 0x0;
	ptr->type = type;
	ptr->segment_type = 1;
	ptr->privilege_level = DPL_KERNEL;
	ptr->present = 1;
	ptr->limit_19_16 = 0xF;
	ptr->soft_use = 0;
	ptr->operation_size = 0;
	ptr->pad0 = 1;
	ptr->granularity = 1;
	ptr->base_31_24  = 0x0;
}

/* This is exactly the same as the one in the bootloader. However the
   previous one cannot be accessed in user process, because its virtual
   address below 0xC0000000, and is mapped into process' private space. */
void
init_segment(void) {
	int i;
	for (i = 0; i < sizeof(gdt); i ++) {
		((uint_8*)gdt)[i] = 0;
	}
	set_segment(&gdt[SEG_KERNEL_CODE], SEG_EXECUTABLE | SEG_READABLE);
	set_segment(&gdt[SEG_KERNEL_DATA], SEG_WRITABLE );
	save_gdt(gdt, sizeof(gdt));
}
