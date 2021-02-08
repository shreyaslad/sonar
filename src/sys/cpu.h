#ifndef __SYS__CPU_H__
#define __SYS__CPU_H__

#include <stdint.h>
#include <stddef.h>
#include <alloc.h>
#include <cpuid.h>
#include <io.h>
#include <str.h>
#include <ospm/apic.h>
#include <sys/msrs.h>
#include <sys/gdt.h>
#include <sys/idt.h>

#undef __MODULE__
#define __MODULE__ "cpu"

#define CPU_BSP 0
#define CPU_AP  1

#define CPU_VENDOR_INTEL    0
#define CPU_VENDOR_AMD      1
#define CPU_VENDOR_UNKNOWN  2

#define CPU_VENDOR_STR_LEN  13

struct cpu_t {
    void* self;

    int vendor;
    char vendor_string[CPU_VENDOR_STR_LEN];

    int ap_type;
    uint32_t lapic_id;
    struct lapic_t* lapic;

    struct {
        uint8_t ept_levels;
        int ept_dirty_accessed;
    } vmx;
};

int cpuid(uint32_t leaf, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d);
int cpuid_count(uint32_t leaf, uint32_t subleaf, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d);

struct cpu_t* get_cpu();

void init_cpu();

#endif
