#ifndef __MEM_H__
#define __MEM_H__

#include <stdint.h>
#include <stddef.h>
#include <log.h>
#include <protos/stivale2.h>
#include <mm/pmm.h>
#include <mm/vmm.h>

#undef __MODULE__
#define __MODULE__ "mem"

#define STACK_SIZE 4096

#define KERNEL_HIGH_VMA 0xffffffff80000000
#define HIGH_VMA        0xffff800000000000

#define TABLESIZE   0x1000
#define PAGESIZE    0x1000

#define RMFLAGS         0x000ffffffffff000
#define GETFLAGS        ~RMFLAGS
#define MAP_TABLEPRESENT    (1 << 0)
#define MAP_TABLEWRITE      (1 << 1)
#define MAP_TABLEUSER       (1 << 2)
#define MAP_TABLEHUGE       (1 << 7)

#define MAP_SUPERVISOR  0
#define MAP_USER        1

void* memset(void* s, int c, size_t n);
int memcmp(const void* s1, const void* s2, size_t n);
void* memcpy(void* dst, const void* src, size_t n);
void* memmove(void* dest, const void* src, size_t n);

void init_mem(struct stivale2_struct_tag_memmap* memmap);

#endif
