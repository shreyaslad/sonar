#include <sys/cpu.h>

#define CPUID_VENDOR_AMD    "AuthenticAMD"
#define CPUID_VENDOR_INTEL  "GenuineIntel"

int get_cpu() {
    uint32_t ebx, ebcx, edx;
}
