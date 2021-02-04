#include <virt/intel/vmx/vmx.h>

#define VMCS_EXIT_ENMI      0
#define VCMS_EXIT_EXTINT    1
#define VCMS_EXIT_TFAULT    2
#define VCMS_EXIT_SINIT     3
#define VCMS_EXIT_SIPI      4
#define VCMS_EXIT_SMI       5
#define VCMS_EXIT_OSMI      6
#define VCMS_EXIT_INTWNDW   7   // RFLAGS.IF = 1 | not blocked by sti/mov
#define VCMS_EXIT_NMIWNDW   8   // no vnmi blocking
#define VCMS_EXIT_TASKSWTCH 9
#define VCMS_EXIT_CPUID     10
#define VCMS_EXIT_GETSEC    11
#define VCMS_EXIT_HLT       12
#define VCMS_EXIT_INVD      13
#define VCMS_EXIT_INVLPG    14
#define VCMS_EXIT_RDPMC     15
#define VCMS_EXIT_RDTSC     16
#define VCMS_EXIT_RSM       17
#define VCMS_EXIT_VMCALL    18
#define VCMS_EXIT_VMCLEAR   19
#define VCMS_EXIT_VMLAUNCH  20
#define VCMS_EXIT_VMPTRLD   21
#define VCMS_EXIT_PTRST     22
#define VCMS_EXIT_VMREAD    23
#define VCMS_EXIT_VMRESUME  24
#define VCMS_EXIT_VMWRITE   25
#define VCMS_EXIT_VMXOFF    26
#define VCMS_EXIT_VMXON     27
#define VCMS_EXIT_CR_ACCESS 28
#define VCMS_EXIT_MOV_DR    29
#define VCMS_EXIT_IO_INS    30
#define VCMS_EXIT_RDMSR     31
#define VCMS_EXIT_WRMSR     32
#define VCMS_EXIT_ENTFAILG  33  // VM-entry failure due to invalid guest state
#define VCMS_EXIT_ENTFAILM  34  // VM-entry failure due to MSR loading
#define VCMS_EXIT_MWAIT     36
#define VCMS_EXIT_MTRAPF    37  // monitor trap flag
#define VCMS_EXIT_MONITOR   38
#define VCMS_EXIT_PAUSE     39
#define VCMS_EXIT_MCHCK     40  // machine check
#define VCMS_EXIT_TPRBT     43  // TPR below threshold
#define VCMS_EXIT_APICACCSS 44
#define VCMS_EXIT_VIRT_EOI  45
#define VCMS_EXIT_GITDR     46  // GDTR or GDTR access
#define VCMS_EXIT_TR        47  // LDTR or TR access
#define VCMS_EXIT_EPTVIO    48
#define VCMS_EXIT_EPTMISCON 49
#define VCMS_EXIT_INVEPT    50
#define VCMS_EXIT_RDTSCP    51
#define VCMS_EXIT_PTE       52  // VMX pre-emption timer expired
#define VCMS_EXIT_INVVPID   53
#define VCMS_EXIT_WBINVD    54
#define VCMS_EXIT_XSETBV    55
#define VCMS_EXIT_APIC_W    56  // APIC write
#define VCMS_EXIT_RDRAND    57
#define VCMS_EXIT_INVPCID   58
#define VCMS_EXIT_VMFUNC    59
#define VCMS_EXIT_ENCLS     60
#define VCMS_EXIT_RDSEED    61
#define VCMS_EXIT_PMLOG_F   62 // page-modification log full
#define VCMS_EXIT_XSAVES    63
#define VCMS_EXIT_XRSTORS   64
#define VCMS_EXIT_SPP       65
#define VCMS_EXIT_UMWAIT    67
#define VCMS_EXIT_TPAUSE    68

const char* vm_instruction_errors[] = {
    "Reserved",
    "VMCALL executed in VMX root operation",
    "VMCLEAR with invalid physical address",
    "VMCLEAR with VMXON pointer",
    "VMLAUNCH with non-clear VMCS",
    "VMRESUME with non-launched VMCS",
    "VMRESUME after VMXOFF",
    "VM entry with invalid control field(s)",
    "VM entry with invalid host-state field(s)",
    "VMPTRLD with invalid physical address",
    "VMPTRLD with VMXON pointer",
    "VMPTRLD with incorrect VMCS revision identifier",
    "VM{READ,WRITE} to unsupported VMCS field",
    "VMWRITE to read-only VMCS field",
    "Reserved",
    "VMXON executed in VMX root operation",
    "VM entry with invalid executive-VMCS pointer",
    "VM entry with non-launched VMCS",
    "VM entry with executive-VMCS pointer not VMXON pointer",
    "VMCALL with non-clear VMCS",
    "VMCALL with invalid VM-exit control field(s)",
    "Reserved",
    "VMCALL with incorrect MSEG revision identifier",
    "VMXOFF under dual-monitor treatment of SMIs and SMM",
    "VMCALL with invalid SMM-monitor features",
    "VM entry with invalid VM-execution control fields in executive VMCS",
    "VM entry with events blocked by MOV SS",
    "Reserved",
    "Invalid operand to INVEPT/INVVPID"
};

static int vmptrld(uint64_t vmcs) {
    uint8_t ret;
    asm volatile (
        "vmptrld %[pa];"
        "setna %[ret];"
        : [ret]"=rm"(ret)
        : [pa]"m"(vmcs)
        : "cc", "memory");
    return ret;
}

static int vmclear(uint64_t vmcs) {
    uint8_t ret;
    asm volatile (
        "vmclear %[pa];"
        "setna %[ret];"
        : [ret]"=rm"(ret)
        : [pa]"m"(vmcs)
        : "cc", "memory");
    return ret;
}

static int vmwrite(uint64_t encoding, uint64_t value) {
    uint8_t ret;
    asm volatile (
        "vmwrite %1, %2;"
        "setna %[ret]"
        : [ret]"=rm"(ret)
        : "rm"(value), "r"(encoding)
        : "cc", "memory");

    return ret;
}

static uint64_t vmread(uint64_t encoding) {
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
    TRACE("VMCS region: %#lx\n", (uint64_t)vmcs + HIGH_VMA);
    memset((void *)((uint64_t)vmcs + HIGH_VMA), 0, PAGESIZE);
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

    //init_vmcs();
}
