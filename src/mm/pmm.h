#ifndef PMM_H
#define PMM_H

#undef __MODULE__
#define __MODULE__ "pmm"

#include <stdint.h>
#include <stddef.h>
#include <protos/limine.h>

#include <mm/common.h>

#include <lib/log.h>
#include <lib/string.h>
#include <lib/bitmap.h>

size_t get_total_mem();
size_t get_usable_mem();

void init_pmm(struct limine_memmap_response* mmap);

#endif
