#ifndef __VMX_H__
#define __VMX_H__

#include <stdint.h>
#include <alloc.h>
#include <mem.h>
#include <mm/pmm.h>
#include <sys/cpu.h>
#include <sys/msrs.h>
#include <log.h>

#undef __MODULE__
#define __MODULE__ "vmx"

void init_vmx();

#endif
