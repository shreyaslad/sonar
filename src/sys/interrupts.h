#ifndef __SYS__INT_H__
#define __SYS__INT_H__

#include <stdint.h>
#include <stddef.h>
#include <alloc.h>
#include <panic.h>
#include <log.h>
#include <trace.h>
#include <sys/regs.h>
#include <sys/idt.h>
#include <sys/cpu.h>
#include <ospm/apic.h>
#include <io.h>

#undef __MODULE__
#define __MODULE__ "int"

#endif
