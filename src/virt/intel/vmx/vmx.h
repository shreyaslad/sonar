#ifndef __VMX_H__
#define __VMX_H__

#include <stdint.h>
#include <alloc.h>
#include <cpuid.h>
#include <mem.h>
#include <sys/msrs.h>
#include <trace.h>
#include <mm/pmm.h>
#include <panic.h>

#undef __MODULE__
#define __MODULE__ "vmx"

/* Virtual Process IDs */

#define HOST_VPID   0
#define GUEST_VPID  1

/* Instruction Wrappers */
int vmptrld(uint64_t vmcs);
int vmclear(uint64_t vmcs);
int vmwrite(uint64_t encoding, uint64_t value);
uint64_t vmread(uint64_t encoding);

void init_vmx();

#endif
