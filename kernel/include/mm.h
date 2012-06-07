#ifndef __MM_H__
#define __MM_H__

#define NR_PPAGE_ENTRY  (((USER_MEM_HIGH)-(USER_MEM_LOW))/(PAGE_SIZE))

#define MM_ALLOCATE	0xA
#define MM_FREE		0xB

extern pid_t	MM;

extern boolean Ppage_flag[NR_PPAGE_ENTRY];

void MemoryManagement(void);

void init_mm(void);

void init_user_page(struct PCB *pcb);

//void allocate_page(struct PCB *pcb, void *start, uint_32 length);

//void free_page(struct PCB *pcb, void *start, uint_32 length);

#endif
