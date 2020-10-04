#ifndef __HYPER_H__
#define __HYPER_H__

#include <stdint.h>
#include <cpuid.h>
#include <trace.h>

#undef __MODULE__
#define __MODULE__ "vmx"

void init_vmx();

#endif