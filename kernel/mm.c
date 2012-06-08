#include "kernel.h"

//#define NR_PPAGE_ENTRY	(((USER_MEM_HIGH)-(USER_MEM_LOW))/(PAGE_SIZE))

pid_t	MM;


boolean Ppage_flag[NR_PPAGE_ENTRY];

void allocate_page(struct PCB *pcb, void *start, uint_32 length);

void free_page(struct PCB *pcb, void *start, uint_32 length);

void MemoryManagement(void) {

	static struct Message m;

	while (TRUE) {
		receive(ANY, &m);
		if (m.type == MM_ALLOCATE)
		{
			allocate_page(m.mm_msg.target_pcb, (void *)m.mm_msg.start, m.mm_msg.length);
			m.type = -1;
			send(m.src, &m);
		}
		else if (m.type == MM_FREE)
		{
			free_page(m.mm_msg.target_pcb, (void *)m.mm_msg.start, m.mm_msg.length);
			m.type = -1;
			send(m.src, &m);
		}
	}

}

void init_mm(void) {
	int i;
	for (i = 0; i < NR_PPAGE_ENTRY; ++ i)
		Ppage_flag[i] = TRUE;

}


//return Physical Address
static inline uint_32 id_to_pa(uint_32 id) {
	return (USER_MEM_LOW + id * PAGE_SIZE);
}


//Physical Address to Physical Memory Flag ID
static inline uint_32 pa_to_id(uint_32 pa) {
	return ((pa - USER_MEM_LOW) / PAGE_SIZE);
}


//allocate physical memory, exclude kernel space, 16MB --> 128MB
//need better algorithm!
static inline void * allocate_memory(void) {
	int i;
	for (i = 0; i < NR_PPAGE_ENTRY; ++ i)
		if (Ppage_flag[i] == TRUE)
		{
			Ppage_flag[i] = FALSE;
			return (void *)id_to_pa(i);
		}
	panic("Physical Page is full!\n");
	return (void *)0;	//just for compile
}

//free physical memory
//need better algorithm!
static inline void free_memory(void *addr) {
	Ppage_flag[pa_to_id((uint_32)addr)] = TRUE;
}


static inline boolean Page_Directory_Fault(struct PageDirectoryEntry *ptr) {
	return ((ptr -> present == 0) ? TRUE : FALSE);
}


static inline boolean Page_Table_Fault(struct PageTableEntry *ptr) {
	return ((ptr -> present == 0) ? TRUE : FALSE);
}


//length:	unit = 4KB
void allocate_page(struct PCB *pcb, void *start, uint_32 length) {

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

	void				*addr;

	int i;
	
	while (length != 0)
	{
		if (Page_Directory_Fault(pdir + pde) == TRUE)
		{
			addr = allocate_memory();
			make_pde(pdir + pde, addr);
			pent = (struct PageTableEntry *)addr;

			init_ptr = pent;
			for (i = 0; i < NR_PTE_ENTRY; ++ i)
			{
				make_invalid_pte(init_ptr);
				++ init_ptr;
			}
		}
		else
			pent = (struct PageTableEntry *)(((pdir + pde) -> page_frame) << 12);
		
		pent = pent + pte;

		for (i = pte; i < NR_PTE_ENTRY; ++ i)
		{
			if (Page_Table_Fault(pent) == FALSE)
				panic("Virtual Page Overlap!\n");

			make_pte(pent, allocate_memory());
			-- length;
			if (length == 0) break;
			++ pent;
		}
		++ pde;
		pte = 0;
	}
	
}


//length:	unit = 4KB
void free_page(struct PCB *pcb, void *start, uint_32 length) {

	/*
	uint_32 pde = ((uint_32*)(((uint_32)(pcb -> cr3)) & ~0xFFF))[((uint_32)start) >> 22];
	uint_32 pte = ((uint_32*)(pde & ~0xFFF))[(((uint_32)start) >> 12) & 0x3FF];
	uint_32 pa = (pte & ~0xFFF) | ((uint_32)start & 0xFFF);
	*/
	
	struct PageDirectoryEntry	*pdir;
	struct PageTableEntry		*pent;

	uint_32 pde = ((uint_32)start) >> 22;
	uint_32 pte = (((uint_32)start) >> 12) & 0x3FF;

	pdir = (struct PageDirectoryEntry *)(pcb -> pagedir);

	boolean clean_pd;
	int i;
	
	while (length != 0)
	{
		if (Page_Directory_Fault(pdir + pde) == TRUE)
			panic("Free Unused Page!\n");
		pent = (struct PageTableEntry *)((pdir + pde) -> page_frame << 12);

		clean_pd = TRUE;
		for (i = 0; i < pte; ++ i)
			if (Page_Table_Fault(pent + i) == FALSE)
			{
				clean_pd = FALSE;
				break;
			}


		for (i = pte; i < NR_PTE_ENTRY; ++ i)
		{
			if (Page_Table_Fault(pent + i) == TRUE)
				panic("Free Unused Page!\n");
			free_memory((void *)((pent + i) -> page_frame << 12));
			make_invalid_pte(pent + i);
			-- length;
			if (length == 0) break;
		}
		

		for ( ; i < NR_PTE_ENTRY; ++ i)
			if (Page_Table_Fault(pent + i) == FALSE)
			{
				clean_pd = FALSE;
				break;
			}


		if (clean_pd == TRUE)
		{
			free_memory((void *)((pdir + pde) -> page_frame << 12));
			make_invalid_pde(pdir + pde);
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

	printk("enter init_user_page\n");

	pdir = (struct PageDirectoryEntry *)(pcb -> pagedir);
	//pent = (struct PageTableEntry *)va_to_pa(pcb -> pagetable);
	

	//PT_offset = (uint_32)va_to_pa(pcb -> pagetable);

	int i, j;

	//initialize page table
	for (i = 0; i < NR_PDE_ENTRY; ++ i)
		make_invalid_pde(pdir + i);

	printk("initialize page table.................Successful!\n");
	//for kernel space
	for (i = 0; i < USER_MEM_LOW / PD_SIZE; ++ i)
	{
		addr = allocate_memory();
		make_pde(pdir + (i + KOFFSET / PD_SIZE), addr);
		pent = (struct PageTableEntry *)addr;
		for (j = 0; j < NR_PTE_ENTRY; ++ j)
		{
			make_pte(pent, (void *)((i * NR_PTE_ENTRY + j) << 12));
			++ pent;
		}
	}

	printk("set kernel space page table...............Successful\n");

	//for user space
	
	//allocate a user stack, below 0xC0000000, 4KB
	allocate_page(pcb, (void *)0xC0000000 - 1, 1);
	
	printk("set user stack page table...............Successful\n");

	*(uint_32*)&(pcb -> cr3) = 0;
	(pcb -> cr3).page_directory_base = ((uint_32)va_to_pa(pdir)) >> 12;
	printk("set CR3 register...............Successful\n");
	printk("exit init_user_page..............Successful\n");
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


/*
static inline void Vpage_init(struct PCB *pcb) {
	int i;
	for (i = 0; i < USER_MEM_HIGH / PD_SIZE; ++ i)
		pcb -> Vpage_flag[i] = TRUE;
}
*/


