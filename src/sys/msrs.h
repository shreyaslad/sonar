#ifndef __SYS__MSRS_H__
#define __SYS__MSRS_H__

#include <stdint.h>
#include <sys/ports.h>

#define MSR_IA32_FEATURE_CONTROL 0x3a

#define MSR_IA32_VMX_BASIC                  0x480
#define MSR_IA32_VMX_PINBASED_CTLS          0x481
#define MSR_IA32_VMX_PROCBASED_CTLS         0x482
#define MSR_IA32_VMX_EXIT_CTLS              0x483
#define MSR_IA32_VMX_ENTRY_CTLS             0x484
#define MSR_IA32_VMX_MISC                   0x485
#define MSR_IA32_VMX_CR0_FIXED0             0x486
#define MSR_IA32_VMX_CR0_FIXED1             0x487
#define MSR_IA32_VMX_CR4_FIXED0             0x488
#define MSR_IA32_VMX_CR4_FIXED1             0x489
#define MSR_IA32_VMX_VMCS_ENUM              0x48a
#define MSR_IA32_VMX_PROCBASED_CTLS2        0x48b
#define MSR_IA32_VMX_EPT_VPID_CAP           0x48c
#define MSR_IA32_VMX_TRUE_PINBASED_CTLS     0x48d
#define MSR_IA32_VMX_TRUE_PROCBASED_CTLS    0x48e
#define MSR_IA32_VMX_TRUE_EXIT_CTLS         0x48f
#define MSR_IA32_VMX_TRUE_ENTRY_CTLS        0x490
#define MSR_IA32_VMX_VMFUNC                 0x491

#define MSR_IA32_EFER 0xC0000080

#define MSR_FS_BASE         0xC0000100
#define MSR_GS_BASE         0xC0000101
#define MSR_KERNEL_GS_BASE  0xC0000102

uint64_t rdmsr(uint64_t msr);
void wrmsr(uint64_t msr, uint64_t data);

#endif
