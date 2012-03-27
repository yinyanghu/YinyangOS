#ifndef __KERNEL_H__
#define __KERNEL_H__

#include "types.h"
#include "const.h"
#include "x86/x86.h"
#include "memory.h"
#include "irq.h"
#include "console.h"
#include "process.h"

void vfprintf(void (*)(char), const char *, void **);

#endif
