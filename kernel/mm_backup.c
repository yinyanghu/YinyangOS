#include "kernel.h"

//#define NR_PPAGE_ENTRY	(((USER_MEM_HIGH)-(USER_MEM_LOW))/(PAGE_SIZE))

pid_t	MM;


uint_8 Ppage_flag[NR_PPAGE_ENTRY];

void allocate_page(struct PCB *, void *, uint_32);

void free_page(struct PCB *, void *, uint_32);

void copy_page(struct PCB *, struct PCB *);

void MemoryManagement(void) {

	static struct Message m;

	while (TRUE) {
		receive(ANY, &m);
		switch (m.type)
		{
			case MM_ALLOCATE:
				allocate_page(m.mm_msg.target_pcb, (void *)m.mm_msg.start, m.mm_msg.length);
				m.type = -1;
				send(m.src, &m);
				break;

			case MM_FREE:
				free_page(m.mm_msg.target_pcb, (void *)m.mm_msg.start, m.mm_msg.length);
				m.type = -1;
				send(m.src, &m);
				break;

			case MM_EXIT_PROC:
				exit_page(m.mm_msg.target_pcb);
				m.type = -1;
				send(m.src, &m);
				break;

			case MM_COPY:
				printk("Starting Fork Memory Copy.................OK!\n");
				copy_page(m.mm_msg.source_pcb, m.mm_msg.target_pcb);
				m.type = -1;
				send(m.src, &m);
				break;
		}

	}

}

void init_MM(void) {
	static int_32	i;
	for (i = 0; i < NR_PPAGE_ENTRY; ++ i)
		Ppage_flag[i] = 0;

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
	static int_32	i;
	for (i = 0; i < NR_PPAGE_ENTRY; ++ i)
		if (Ppage_flag[i] == 0)
		{
			Ppage_flag[i] = 1;
			return (void *)id_to_pa(i);
		}
	panic("Physical Page is full!\n");
	return (void *)0;	//just for compile
}

//free physical memory
//need better algorithm!
static inline void free_memory(void *addr) {
//	panic("Stop Free!\n");
	-- Ppage_flag[pa_to_id((uint_32)addr)];
}

//copy physical memory from source address to target address
//unit = 4KB
static void copy_memory(uint_32 src, uint_32 dest) {
	static int_32	i;	
	printk("Source Address = %x, Target Address = %x\n", src, dest);
	for (i = 0; i < (PAGE_SIZE ); ++ i)
		*((uint_8 *)dest + i) = *((uint_8 *)src + i);
		//printk("%x\n", *((uint_8 *)dest + i));

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
	
	static struct PageDirectoryEntry	*pdir;
	static struct PageTableEntry		*pent, *init_ptr;

	uint_32 pde = ((uint_32)start) >> 22;
	uint_32 pte = (((uint_32)start) >> 12) & 0x3FF;

	pdir = (struct PageDirectoryEntry *)(pcb -> pagedir);

	static void		*addr;

	static int_32	i;
	
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

void exit_page(struct PCB *pcb) {
	
	static struct PageDirectoryEntry	*pdir;
	static struct PageTableEntry		*pent;

	static int_32	i, j;

	pdir = (struct PageDirectoryEntry *)(pcb -> pagedir);

	for (i = 0; i < NR_PDE_ENTRY; ++ i)
		if (Page_Directory_Fault(pdir + i) == FALSE)
		{
			pent = (struct PageTableEntry *)(((pdir + i) -> page_frame) << 12);
			for (j = 0; j < NR_PTE_ENTRY; ++ j)
				if (Page_Table_Fault(pent + j) == FALSE)
				{
					free_memory((void *)((pent + j) -> page_frame << 12));
					make_invalid_pte(pent + j);

				}
			make_invalid_pde(pdir + i);
		}

}


void copy_page(struct PCB *source, struct PCB *target) {
	static struct PageDirectoryEntry	*pdir_source, *pdir_target;
	static struct PageTableEntry		*pent_source, *pent_target;

	static void		*addr;

	static int_32	i, j;

	pdir_source = (struct PageDirectoryEntry *)(source -> pagedir);
	pdir_target = (struct PageDirectoryEntry *)(target -> pagedir);

	printk("Enter Copy Page.....................OK!\n");
	printk("pcb = %d %d\n", source -> pid , target -> pid);

	/*
	for (i =USER_MEM_LOW; i < USER_MEM_HIGH; i += 4)
		if (*(uint_32 *)i != 0)
		{
			printk("%x %x\n", i, *(uint_32 *)i);
			//panic("first blood!\n");
		}
		*/

	//panic("Prophet\n");
	
	//for (i = 0x1009000; i < 0x100A000; ++ i)
	//		printk("%x %x\n", i, *(uint_32 *)i);

	for (i = 0; i < (NR_PDE_ENTRY >> 2) * 3; ++ i)
		if (Page_Directory_Fault(pdir_source + i) == FALSE)
		{
			printk("i = %d\n", i);
			pent_source = (struct PageTableEntry *)(((pdir_source + i) -> page_frame) << 12);

			addr = allocate_memory();
			make_pde(pdir_target + i, addr);
			pent_target = (struct PageTableEntry *)addr;
			for (j = 0; j < NR_PTE_ENTRY; ++ j)
				if (Page_Table_Fault(pent_source + j) == FALSE)
				{
				//	printk("j = %d\n", j);
					if ((((pent_source + j) -> page_frame) << 12) >= USER_MEM_LOW)
					{
						addr = allocate_memory();
						make_pte(pent_target + j, addr);
						copy_memory(((pent_source + j) -> page_frame) << 12, (uint_32)addr);
						printk("PA = %x\n", ((pent_source + j) -> page_frame) << 12);
					}
					else
					{
				//		panic("kernel");
		//				printk("=================================================\n");
						make_pte(pent_target + j, (void *)(((pent_source + j) -> page_frame) << 12));
					}

				}
				else
					make_invalid_pte(pent_target + j);
	//if (i > 50)panic("first blood!\n");
		}
		else
			make_invalid_pde(pdir_target + i);

	//panic("sdfsdfsdfsdfsdfsfadfasdfas\n");
	
}

//length:	unit = 4KB
void free_page(struct PCB *pcb, void *start, uint_32 length) {

	/*
	uint_32 pde = ((uint_32*)(((uint_32)(pcb -> cr3)) & ~0xFFF))[((uint_32)start) >> 22];
	uint_32 pte = ((uint_32*)(pde & ~0xFFF))[(((uint_32)start) >> 12) & 0x3FF];
	uint_32 pa = (pte & ~0xFFF) | ((uint_32)start & 0xFFF);
	*/
	
	static struct PageDirectoryEntry	*pdir;
	static struct PageTableEntry		*pent;

	uint_32 pde = ((uint_32)start) >> 22;
	uint_32 pte = (((uint_32)start) >> 12) & 0x3FF;

	pdir = (struct PageDirectoryEntry *)(pcb -> pagedir);

	static boolean		clean_pd;
	static int_32		i;
	
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

	static struct PageDirectoryEntry	*pdir;
	static struct PageTableEntry		*pent;

	static void				*addr;

//	uint_32				PT_offset;

	printk("enter init_user_page\n");

	pdir = (struct PageDirectoryEntry *)(pcb -> pagedir);
	//pent = (struct PageTableEntry *)va_to_pa(pcb -> pagetable);
	

	//PT_offset = (uint_32)va_to_pa(pcb -> pagetable);

	static int_32	 i, j;

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

	*(uint_32 *)&(pcb -> cr3) = 0;
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


