#ifndef __MM_H__
#define __MM_H__

#define NR_PPAGE_ENTRY  (((USER_MEM_HIGH)-(USER_MEM_LOW))/(PAGE_SIZE))

#define MM_ALLOCATE		0xB0
#define MM_FREE			0xB1
#define MM_EXIT_PROC	0xB2
#define MM_COPY			0xB3
#define MM_EXEC			0xB4

extern pid_t	MM;

extern boolean Ppage_flag[];

void MemoryManagement(void);

void init_MM(void);

void init_user_page(struct PCB *);

//void allocate_page(struct PCB *pcb, void *start, uint_32 length);

//void free_page(struct PCB *pcb, void *start, uint_32 length);

#endif
