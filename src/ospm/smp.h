#ifndef __SYS__SMP_H__
#define __SYS__SMP_H__

#include <stdint.h>
#include <stdatomic.h>
#include <protos/stivale2.h>
#include <alloc.h>
#include <mem.h>
#include <locks.h>
#include <sys/gdt.h>
#include <sys/cpu.h>
#include <ospm/apic.h>

#undef __MODULE__
#define __MODULE__ "smp"

void init_smp(struct stivale2_struct_tag_smp* smp);

#endif
