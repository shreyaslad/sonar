#ifndef __MM__VMM_H__
#define __MM__VMM_H__

#include <stdint.h>
#include <stddef.h>
#include <io.h>
#include <mem.h>
#include <mm/pmm.h>
#include <str.h>

uint64_t* get_pml4();
void set_pml4(uint64_t pml4);
void invlpg(uint64_t* vaddr);
void tlbflush();

void vmm_map(uint64_t* vaddr, uint64_t* paddr, uint64_t* pml4ptr, uint64_t permission);
void vmm_unmap(uint64_t* vaddr, size_t pages);

void test();

#endif
