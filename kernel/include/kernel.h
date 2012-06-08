#ifndef __KERNEL_H__
#define __KERNEL_H__

#include "types.h"
#include "const.h"
#include "x86/x86.h"
#include "memory.h"
#include "irq.h"
#include "console.h"
#include "lock.h"
#include "semaphore.h"
#include "message.h"
#include "drivers/driver.h"
#include "process.h"
#include "fm.h"
#include "mm.h"
#include "pm.h"
#include "test/myproc.h"
#include "test/test_proc.h"

void vfprintf(void (*)(char), const char *, void **);

#endif
