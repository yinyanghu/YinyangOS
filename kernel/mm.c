#include "kernel.h"

#define NR_PPAGE_ENTRY	(((USER_MEM_HIGH)-(USER_MEM_LOW))/(PAGE_SIZE))

pid_t	MM;


boolean Ppage_flag [NR_PPAGE_ENTRY];

void MemoryManagement(void) {

	static struct Message m;

}

static inline void Ppage_init(void) {
	int i;
	for (i = 0; i < NR_PPAGE_ENTRY; ++ i)
		Ppage_flag[i] = TRUE;

}

/*
static inline void Vpage_init(struct PCB *pcb) {
	int i;
	for (i = 0; i < USER_MEM_HIGH / PD_SIZE; ++ i)
		pcb -> Vpage_flag[i] = TRUE;
}
*/


//return Physical Address
static inline uint_32 PhysicalPage(uint_32 page_id) {
	return (USER_MEM_LOW + page_id * PAGE_SIZE);
}


/*
//return Physical Address, without PageTable Offset
static inline uint_32 VirtualPage(uint_32 page_id) {
	return (page_id * PD_SIZE);
}
*/

//exclude kernel space, 16MB --> 128MB
//need better algorithm!
static inline uint_32 Find_Empty_Ppage(void) {
	int i;
	for (i = 0; i < NR_PPAGE_ENTRY; ++ i)
		if (Ppage_flag[i] == TRUE)
		{
			Ppage_flag[i] = FALSE;
			return PhysicalPage(i);
		}
	panic("Physical Page is full!\n");
}

/*
static inline uint_32 Find_Empty_Vpage(struct PCB *pcb) {
	int i;
	for (i = 0; i < USER_MEM_HIGH / PD_SIZE; ++ i)
		if (pcb -> Vpage_flag[i] == TRUE)
		{
			Vpage_flag[i] = FALSE;
			return VirtualPage(i);
		}
	panic("Virtual Page is full!\n");
}
*/



static inline boolean Page_Directory_Fault(struct PageDirectoryEntry *ptr) {
	return ((ptr -> present == 0) ? TRUE : FALSE);
}


static inline boolean Page_Table_Fault(struct PageTableEntry *ptr) {
	return ((ptr -> present == 0) ? TRUE : FALSE);
}


//length:	unit = 4KB
void allocate(struct PCB *pcb, void *start, uint_32 length) {

	/*
	uint_32 pde = ((uint_32*)(((uint_32)(pcb -> cr3)) & ~0xFFF))[((uint_32)start) >> 22];
	uint_32 pte = ((uint_32*)(pde & ~0xFFF))[(((uint_32)start) >> 12) & 0x3FF];
	uint_32 pa = (pte & ~0xFFF) | ((uint_32)start & 0xFFF);
	*/
	
	struct PageDirectoryEntry	*pdir;
	struct PageTableEntry		*pent, *init_ptr;

	uint_32 pde = ((uint_32)start) >> 22;
	uint_32 pte = (((uint_32)start) >> 12) & 0x3FF;

	pdir = (struct PageDirectoryEntry *)(pcb -> pagedir);
	
	while (length != 0)
	{
		if (Page_Directory_Fault(pdir[pde]))
		{
			addr = (void *)(Find_Empty_Ppage());
			make_pde(&pdir[pde], addr);
			pent = (struct PageTableEntry *)addr;

			init_ptr = pent;
			for (i = 0; i < NR_PTE_ENTRY; ++ i)
			{
				make_invalid_pte(init_ptr);
				++ init_ptr;
			}
		}
		else
			pent = (struct PageTableEntry *)(pdir[pde].page_frame << 12);
		
		pent = pent + pte;

		for (j = pte; j < NR_PTE_ENTRY; ++ j)
		{
			if (!Page_Table_Fault(pent))
				panic("Virtual Page Overlap!\n");

			make_pte(pent, (void *)Find_Empty_Ppage());
			-- length;
			if (length == 0) break;
			++ pent;
		}
		++ pde;
		pte = 0;
	}
	
}

void init_user_page(struct PCB *pcb) {

	struct PageDirectoryEntry	*pdir;
	struct PageTableEntry		*pent;

	void				*addr;

//	uint_32				PT_offset;

	pdir = (struct PageDirectoryEntry *)(pcb -> pagedir);
	//pent = (struct PageTableEntry *)va_to_pa(pcb -> pagetable);
	

	//PT_offset = (uint_32)va_to_pa(pcb -> pagetable);

	int i, j;

	//initialize page table
	for (i = 0; i < NR_PDE_ENTRY; ++ i)
		make_invalid_pde(&pdir[i]);

	//for kernel space
	for (i = 0; i < USER_MEM_LOW / PD_SIZE; ++ i)
	{
		addr = (void *)(Find_Empty_Ppage());
		make_pde(&pdir[i + KOFFSET / PD_SIZE], addr);
		pent = (struct PageTableEntry *)pa_to_va(addr);
		for (j = 0; j < NR_PTE_ENTRY; ++ j)
		{
			make_pte(pent, (void *)((i * NR_PTE_ENTRY + j) << 12));
			++ pent;
		}
	}

	//for user space
	
	//allocate a user stack, below 0xC0000000, 4KB
	
	/*
	addr = (void *)(Find_Empty_Ppage());
	make_pde(&pdir[KOFFSET / PD_SIZE - 1], addr);
	pent = (struct PageTableEntry *)pa_to_va(addr);
	for (j = 0; j < NR_PTE_ENTRY - 1; ++ j)
	{
		make_invalid_pte(pent);
		++ pent;
	}

	make_pte(pent, (void *)(Find_Empty_Ppage()));

	*/

	*(uint_32*)&(pcb -> cr3) = 0;
	(pcb -> cr3).page_directory_base = ((uint_32)va_to_pa(pdir)) >> 12;
}


