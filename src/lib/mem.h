#ifndef __MEM_H__
#define __MEM_H__

#include <stdint.h>
#include <stddef.h>
#include <trace.h>
#include <boot/stivale2.h>
#include <mm/pmm.h>
#include <mm/vmm.h>

#undef __MODULE__
#define __MODULE__ "mem"

#define KERNEL_HIGH_VMA 0xFFFFFFFF80000000
#define HIGH_VMA        0xFFFF800000000000

#define TABLESIZE   0x1000
#define PAGESIZE    0x1000

#define RMFLAGS         0x000FFFFFFFFFF000
#define GETFLAGS        ~RMFLAGS
#define TABLEPRESENT    (1 << 0)
#define TABLEWRITE      (1 << 1)
#define TABLEUSER       (1 << 2)
#define TABLEHUGE       (1 << 7)

#define SUPERVISOR  0
#define USER        1

void* memcpy(void* dest, const void* src, size_t size);
void* memset(void* bufptr, int value, size_t size);
int memcmp(const void* s1, const void* s2, size_t n);

void init_mem(struct stivale2_struct_tag_memmap* memmap);

#endif
