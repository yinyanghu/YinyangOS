#ifndef __X86_MEMORY_H__
#define __X86_MEMORY_H__

/* 32bit x86 uses 4KB page size */
#define PAGE_SIZE                  4096
#define NR_PDE_ENTRY               1024
#define NR_PTE_ENTRY               1024
#define PD_SIZE                    ((NR_PTE_ENTRY) * (PAGE_SIZE))

/* force the data to be aligned with page boundary.
   statically defined page tables uses this feature. */
#define align_to_page              __attribute((aligned(PAGE_SIZE)))

/* the 32bit Page Directory(first level page table) data structure */
struct PageDirectoryEntry {
	uint_32 present             : 1;
	uint_32 read_write          : 1; 
	uint_32 user_supervisor     : 1;
	uint_32 page_write_through  : 1;
	uint_32 page_cache_disable  : 1;
	uint_32 accessed            : 1;
	uint_32 pad0                : 6;
	uint_32 page_frame          : 20;
};

/* the 32bit Page Table Entry(second level page table) data structure */
struct PageTableEntry {
	uint_32 present             : 1;
	uint_32 read_write          : 1;
	uint_32 user_supervisor     : 1;
	uint_32 page_write_through  : 1;
	uint_32 page_cache_disable  : 1;
	uint_32 accessed            : 1;
	uint_32 dirty               : 1;
	uint_32 pad0                : 1;
	uint_32 global              : 1;
	uint_32 pad1                : 3;
	uint_32 page_frame          : 20;
};

/* the 64bit segment descriptor */
struct SegmentDescriptor {
	uint_32 limit_15_0          : 16;
	uint_32 base_15_0           : 16;
	uint_32 base_23_16          : 8;
	uint_32 type                : 4;
	uint_32 segment_type        : 1;
	uint_32 privilege_level     : 2;
	uint_32 present             : 1;
	uint_32 limit_19_16         : 4;
	uint_32 soft_use            : 1;
	uint_32 operation_size      : 1;
	uint_32 pad0                : 1;
	uint_32 granularity         : 1;
	uint_32 base_31_24          : 8;
};

#define SEG_CODEDATA            1
#define SEG_32BIT               1
#define SEG_4KB_GRANULARITY     1
#define SEG_TSS_32BIT           0x9

#define DPL_KERNEL              0
#define DPL_USER                3

#define SEG_WRITABLE            0x2
#define SEG_READABLE            0x2
#define SEG_EXECUTABLE          0x8

#define NR_SEGMENTS             3
#define SEG_KERNEL_CODE         1 
#define SEG_KERNEL_DATA         2

struct GateDescriptor {
	uint_32 offset_15_0      : 16;
	uint_32 segment          : 16;
	uint_32 pad0             : 8;
	uint_32 type             : 4;
	uint_32 system           : 1;
	uint_32 privilege_level  : 2;
	uint_32 present          : 1;
	uint_32 offset_31_16     : 16;
};

struct TrapFrame {
	uint_32 edi, esi, ebp, xxx, ebx, edx, ecx, eax;
	int_32 irq;
};

#endif
