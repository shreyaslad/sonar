#ifndef __HYPER__VMX_H__
#define __HYPER__VMX_H__

#include <stdint.h>
#include <cpuid.h>
#include <trace.h>

#undef __MODULE__
#define __MODULE__ "vmx"

/* Fields */

#define VM_E_INS            0b000
#define VM_E_RES            0b001
#define VM_E_INT_INFO       0b010
#define VM_E_INT_CODE       0b011
#define VM_E_IDT_VEC_INFO   0b100
#define VM_E_IDT_VEC_CODE   0b101
#define VM_E_INS_LEN        0b110
#define VM_E_INS_INFO       0b111

extern uint64_t vmread(uint64_t field);
extern void vmwrite(uint64_t field, uint64_t* data);

int init_vmx();

#endif
