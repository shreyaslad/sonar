#ifndef __SYS__SMP_H__
#define __SYS__SMP_H__

#include <stdint.h>
#include <boot/stivale2.h>
#include <mem.h>
#include <alloc.h>

void init_smp(struct stivale2_struct_tag_smp* smp_info);

#endif
