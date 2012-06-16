#ifndef __IDE_H__
#define __IDE_H__

#define IDE_PORT_CTRL   0x3F6
#define IDE_PORT_BASE   0x1F0

#define IDE_READY		0x10
#define IDE_WRITEBACK	0x11

#define WRITEBACK_TIME  100  /* writeback cache for every 1 second */

#define NR_SEC_CACHE	127   /* sector cache is a direct address hash */

extern pid_t IDE;

void ide_driver_initialize(void);
void ide_driver_thread(void);

#endif
