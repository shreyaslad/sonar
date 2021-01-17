#ifndef __SYS__SMP_H__
#define __SYS__SMP_H__

#include <stdint.h>
#include <protos/stivale2.h>
#include <mem.h>
#include <alloc.h>

#undef __MODULE__
#define __MODULE__ "smp"

void init_smp(struct stivale2_struct_tag_smp* smp);

#endif
