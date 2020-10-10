#ifndef __SYS__MSRS_H__
#define __SYS__MSRS_H__

#include <stdint.h>
#include <sys/ports.h>

#define MSR_CODE_IA32_FEATURE_CONTROL   0x3a
#define MSR_CODE_IA32_VMX_BASIC         0x480
#define MSR_CODE_IA32_VMX_CR0_FIXED0    0x486
#define MSR_CODE_IA32_VMX_CR0_FIXED1    0x487
#define MSR_CODE_IA32_VMX_CR4_FIXED0    0x488
#define MSR_CODE_IA32_VMX_CR4_FIXED1    0x489

uint64_t rdmsr(uint64_t msr);
void wrmsr(uint64_t msr, uint64_t data);

#endif
