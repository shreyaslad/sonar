#include <sys/cpu.h>

#define CPUID_VENDOR_INTEL  "GenuineIntel"
#define CPUID_VENDOR_AMD    "AuthenticAMD"

int cpuid(uint32_t leaf, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d) {
    return __get_cpuid(leaf, a, b, c, d);
}

int cpuid_count(uint32_t leaf, uint32_t subleaf, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d) {
    return __get_cpuid_count(leaf, subleaf, a, b, c, d);
}

struct cpu_t* get_cpu() {
    struct cpu_t* ret = 0;
    asm volatile("mov %%gs:0, %0"
                 : "=r"(ret));

    return ret;
}

void init_cpu() {
    struct cpu_t* cpu = kmalloc(sizeof(struct cpu_t));

    cpu->self = cpu;
    wrmsr(MSR_GS_BASE, (uint64_t)cpu);

    set_gdt();
    set_idt();

    uint32_t ebx, ecx, edx;

    cpuid(0, 0, &ebx, &ecx, &edx);

    snprintf(cpu->vendor_string, CPU_VENDOR_STR_LEN, "%.4s%.4s%.4s",
             &ebx, &edx, &ecx);

    //TRACE("Vendor ID: %s\n", cpu->vendor_string);

    if (!strcmp(cpu->vendor_string, CPUID_VENDOR_INTEL)) {
        cpu->vendor = CPU_VENDOR_INTEL;
    } else if (!strcmp(cpu->vendor_string, CPUID_VENDOR_AMD)) {
        cpu->vendor = CPU_VENDOR_AMD;
    } else {
        cpu->vendor = CPU_VENDOR_UNKNOWN;
    }
}
