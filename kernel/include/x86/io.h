#ifndef __X86_IO_H__
#define __X86_IO_H__

#include "kernel.h"

static inline uint_8
in_byte(uint_16 port) {
	uint_8 data;
	asm volatile("in %1, %0" : "=a"(data) : "d"(port));
	return data;
}

static inline uint_32
in_long(uint_16 port) {
	uint_32 data;
	asm volatile("in %1, %0" : "=a"(data) : "d"(port));
	return data;
}

static inline void
out_byte(uint_16 port, uint_8 data) {
	asm volatile("out %%al, %%dx" : : "a"(data), "d"(port));
}

static inline void
out_long(uint_16 port, uint_32 data) {
	asm volatile("out %%eax, %%dx" : : "a"(data), "d"(port));
}

#endif
