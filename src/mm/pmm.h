#ifndef __MM__PMM_H__
#define __MM__PMM_H__

#include <stdint.h>
#include <stddef.h>
#include <io.h>
#include <bit.h>
#include <lib/mem.h>

extern uint64_t totalmem;
extern volatile uint64_t* pmm_bitmap;

/* Physical Memory Allocation */
void* pmm_alloc(size_t pages);
void pmm_free(void* ptr, size_t pages);
void* pmm_realloc(void* ptr, size_t old, size_t new);

#endif
