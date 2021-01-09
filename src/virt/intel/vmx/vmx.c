#include <virt/intel/vmx/vmx.h>

int vmptrld(uint64_t vmcs) {
    uint8_t ret;
    asm volatile (
        "vmptrld %[pa];"
        "setna %[ret];"
        : [ret]"=rm"(ret)
        : [pa]"m"(vmcs)
        : "cc", "memory");
    return ret;
}

int vmclear(uint64_t vmcs) {
    uint8_t ret;
    asm volatile (
        "vmclear %[pa];"
        "setna %[ret];"
        : [ret]"=rm"(ret)
        : [pa]"m"(vmcs)
        : "cc", "memory");
    return ret;
}

int vmwrite(uint64_t encoding, uint64_t value) {
    uint8_t ret;
    asm volatile (
        "vmwrite %1, %2;"
        "setna %[ret]"
        : [ret]"=rm"(ret)
        : "rm"(value), "r"(encoding)
        : "cc", "memory");

    return ret;
}

uint64_t vmread(uint64_t encoding) {
    uint64_t tmp;
    uint8_t ret;
    asm volatile(
        "vmread %[encoding], %[value];"
        "setna %[ret];"
        : [value]"=rm"(tmp), [ret]"=rm"(ret)
        : [encoding]"r"(encoding)
        : "cc", "memory");

    return tmp;
}

static int init_vmcs() {
    uint64_t* vmcs = pmm_alloc(1);
    TRACE("VMCS region: %#lp\n", vmcs + HIGH_VMA);
    memset(vmcs + HIGH_VMA, 0, PAGESIZE);
    vmptrld((uint64_t)vmcs);

    // Set execution state

}

static int vmxon() {
    void* vmxon_region = pmm_alloc(1);
    TRACE("VMXON region: %#lx\n", (size_t)vmxon_region + HIGH_VMA);
    memset(vmxon_region + HIGH_VMA, 0, PAGESIZE);

    size_t control = rdmsr(MSR_CODE_IA32_FEATURE_CONTROL);

    if ((control & (0x1 | 0x4)) != (0x1 | 0x4)) {
        wrmsr(MSR_CODE_IA32_FEATURE_CONTROL, control | 0x1 | 0x4);
    }

    size_t cr0;
    asm volatile(
        "mov %%cr0, %0\n\t"
        : "=r"(cr0));
    cr0 &= rdmsr(MSR_CODE_IA32_VMX_CR0_FIXED1);
    cr0 |= rdmsr(MSR_CODE_IA32_VMX_CR0_FIXED0);
    asm volatile(
        "mov %0, %%cr0\n\t"
        :
        : "r"(cr0));
    
    uint64_t cr4;
    asm volatile("mov %%cr4, %0\n\t"
                : "=r"(cr4));
    cr4 |= (1 << 13);
    cr4 &= rdmsr(MSR_CODE_IA32_VMX_CR4_FIXED1);
    cr4 |= rdmsr(MSR_CODE_IA32_VMX_CR4_FIXED0);
    asm volatile(
        "mov %0, %%cr4\n\t"
        :
        : "r"(cr4));

    uint32_t vmx_rev = rdmsr(MSR_CODE_IA32_VMX_BASIC);

    uint8_t successful;
    asm volatile(
        "vmxon %1\n\t"
        "jnc success\n\t"
        "movq $0, %%rax\n"
        "success:\n\t"
        "movq $1, %%rax\n\t"
        : "=a"(successful)
        : "m"(vmxon_region)
        : "memory", "cc");

    if (successful) {
        TRACE("Entered VMXON operation\n");
    } else {
        ERR("VMXON operation failed!\n");
        return 0;
    }

    return 1;
}

static int has_vmx() {
    uint32_t eax = (1 << 5);
    uint32_t ecx, unused;
    __get_cpuid(1, &eax, &unused, &ecx, &unused);

    return ecx & 1;
}

void init_vmx() {
    if (!has_vmx()) {
        panic("CPU does not support VMX!");
    }

    TRACE("CPU supports VMX\n");

    if (!vmxon()) {
        panic("Failed to execute VMXON!");
    }
}
