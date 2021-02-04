#ifndef __SYS__CPU_H__
#define __SYS__CPU_H__

#include <stdint.h>
#include <cpuid.h>

#define CPU_INTEL   0
#define CPU_AMD     1

int get_cpu();

#endif
