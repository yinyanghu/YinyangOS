#include "kernel.h"

#define IDE_PORT_CTRL   0x3F6
#define IDE_PORT_BASE   0x1F0
#define IDE_READY       1
#define IDE_WRITEBACK   2
#define WRITEBACK_TIME  100  /* writeback cache for every 1 second */

#define NR_SEC_CACHE    127   /* sector cache is a direct address hash */

pid_t IDE;

static void ide_intr(void);
static void time_intr(void);

static void issue_read(void);
static void do_read(void *);
static void do_write(void *);
static void ide_prepare(uint_32);
static uint_8 read_byte(uint_32);
static void write_byte(uint_32, uint_8);

struct SectorCache {
	uint_32 sector;
	boolean used, dirty;
	uint_8  content[512];
};
static struct SectorCache cache[NR_SEC_CACHE];
static void cache_init(void);

void
ide_driver_thread(void) {
	static struct Message m;

	add_irq_handle(14, ide_intr);
	add_irq_handle(0 , time_intr);
	cache_init();

	while (TRUE) {
		receive(ANY, &m);
		if (m.src == MSG_HARD_INTR) {
			if (m.type == IDE_WRITEBACK) {
				int i;
				for (i = 0; i < NR_SEC_CACHE; i ++) {
					if (cache[i].dirty == TRUE) {
						ide_prepare(cache[i].sector);
						do_write(&cache[i].content);
						cache[i].dirty = FALSE;
					}
				}
			} else {
				panic("IDE interrupt is leaking");
			}
		} else if (m.type == DEV_READ) {
			uint_32 i, data;
			struct PCB *pcb = fetch_pcb(m.src);
			for (i = 0; i < m.dev_io.length; i ++) {
				data = read_byte(m.dev_io.offset + i);
				copy_from_kernel(pcb, m.dev_io.buf + i, &data, 1);
			}
			m.type = -1;
			m.int_msg.p1 = i;
			send(m.src, &m);
		} else if (m.type == DEV_WRITE) {
			uint_32 i, data;
			struct PCB *pcb = fetch_pcb(m.src);
			for (i = 0; i < m.dev_io.length; i ++) {
				copy_to_kernel(pcb, &data, m.dev_io.buf + i, 1);
				write_byte(m.dev_io.offset + i, data);
			}
			m.type = -1;
			m.int_msg.p1 = i;
			send(m.src, &m);
		}
	}
}

static void
ide_intr(void) {
	static struct Message m;
	m.type = IDE_READY;
	send(IDE, &m);
}
static void
time_intr(void) {
	static struct Message m;
	static uint_32 counter = 0;
	counter = (counter + 1) % WRITEBACK_TIME;
	if (counter == 0) {
		m.type = IDE_WRITEBACK;
		send(IDE, &m);
	}
}

static void
ide_prepare(uint_32 sector) {
	int r;
	r = in_byte(IDE_PORT_BASE + 7);
	if ( (r & (0x80 | 0x40)) != 0x40)
		panic("IDE NOT READY");

	out_byte(IDE_PORT_CTRL, 0);
	out_byte(IDE_PORT_BASE + 2, 1);
	out_byte(IDE_PORT_BASE + 3, sector & 0xFF);
	out_byte(IDE_PORT_BASE + 4, (sector >> 8) & 0xFF);
	out_byte(IDE_PORT_BASE + 5, (sector >> 16) & 0xFF);
	out_byte(IDE_PORT_BASE + 6, 0xE0 | ((sector >> 24) & 0xFF));
}

static void
issue_read(void) {
	out_byte(IDE_PORT_BASE + 7, 0x20);
}

static void
do_read(void *buf) {
	int i;
	static struct Message m;
	do {
		receive(MSG_HARD_INTR, &m);
	} while (m.type != IDE_READY);

	for (i = 0; i < 512 / sizeof(uint_32); i ++) {
		*(((uint_32*)buf) + i) = in_long(IDE_PORT_BASE);
	}
}

static void
do_write(void *buf) {
	int i;
	static struct Message m;
	out_byte(IDE_PORT_BASE + 7, 0x30);
	for (i = 0; i < 512 / sizeof(uint_32); i ++) {
		out_long(IDE_PORT_BASE, *(((uint_32*)buf) + i));
	}
	do {
		receive(MSG_HARD_INTR, &m);
	} while (m.type != IDE_READY);
}

static void
cache_init(void) {
	int i;
	for (i = 0; i < NR_SEC_CACHE; i ++) {
		cache[i].used = FALSE;
	}
}

static struct SectorCache *
cache_fetch(uint_32 sector) {
	struct SectorCache *ptr = &cache[sector % NR_SEC_CACHE];

	if (ptr->used == TRUE && ptr->sector == sector) {
		/* cache hit, do nothing */
	} else {
		if (ptr->used == TRUE && ptr->dirty == TRUE) {
			/* write back */
			ide_prepare(ptr->sector);
			do_write(&ptr->content);
		}
		/* issue a read command */
		ide_prepare(sector);
		issue_read();
		do_read(&ptr->content);
		ptr->used = TRUE;
		ptr->sector = sector;
		ptr->dirty = FALSE;
	}
	return ptr;
}

static uint_8
read_byte(uint_32 offset) {
	uint_32 sector = offset >> 9;
	struct SectorCache *ptr = cache_fetch(sector);
	return ptr->content[offset & 511];
}
static void
write_byte(uint_32 offset, uint_8 data) {
	uint_32 sector = offset >> 9;
	struct SectorCache *ptr = cache_fetch(sector);
	ptr->content[offset & 511] = data;
	ptr->dirty = TRUE;
}

