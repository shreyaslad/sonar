#ifndef __ELF_H__
#define __ELF_H__

#include <stdint.h>
#include <stddef.h>
#include <alloc.h>
#include <mem.h>
#include <panic.h>
#include <str.h>
#include <log.h>
#include <protos/stivale2.h>

#define __MODULE__ "elf"

int elf_load(uint64_t* module, uint64_t* entry);

#endif
