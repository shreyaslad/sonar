#include <hyper/vmx.h>

int has_vmx() {
    int eax = (1 << 5);
    int ecx, unused;
    __get_cpuid(1, &eax, &unused, &ecx, &unused);

    return ecx & 1;
}
