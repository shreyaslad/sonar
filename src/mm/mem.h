#ifndef MEM_H
#define MEM_H

#include <stdint.h>

#include <protos/limine.h>
#include <mm/pmm.h>
#include <lib/log.h>

#undef __MODULE__
#define __MODULE__ "mm"

void init_mem(struct limine_memmap_response* mmap);

#endif
