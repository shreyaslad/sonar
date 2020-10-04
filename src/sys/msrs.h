#ifndef __SYS__MSRS_H__
#define __SYS__MSRS_H__

#include <stdint.h>
#include <sys/ports.h>

uint64_t rdmsr(uint64_t msr);
void wrmsr(uint64_t msr, uint64_t data);

#endif
