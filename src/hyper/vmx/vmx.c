#include <hyper/vmx/vmx.h>

static
uint32_t has_vmx() {
    uint32_t eax = (1 << 5);
    uint32_t ecx, unused;
    __get_cpuid(1, &eax, &unused, &ecx, &unused);

    return ecx & 1;
}

int init_vmx() {
    if (!has_vmx()) {
        ERR("CPU does not support VMX!");
        return 0;
    }

    TRACE("CPU supports VMX\n");

    // set CR4.VMXE
    asm volatile("movq %cr4, %rax\n\t"
                 "orq (1 << 13), %rax\n\t"
                 "movq %rax, %cr4\n\t");

    // TODO: everything else

    return 1;
}
