#include <hyper/vmx/vmx.h>

struct vmcs_region_t {
    uint32_t rev_id: 31;
    uint8_t s_vmcs_id: 1;
    uint32_t vmx_abort_ind;
    uint32_t vmcs_data[];
} __attribute__((packed));

#define GSTATE_ACTIVITY_ACTIVE  0
#define GSTATE_ACTIVITY_HLT     1
#define GSTATE_ACTIVITY_SHUTDWN 2
#define GSTATE_ACTIVITY_WSIPI   3

struct vmcs_segment_t {
    uint64_t base;
    uint32_t limit;
} __attribute((packed));

/* VMCS Selector Fields */
struct vmcs_selectors_t {
    uint16_t selector;
    struct vmcs_segment_t segment;
    
    struct access_rights_t {
        uint8_t seg_type: 4;
        uint8_t desc_type: 1;
        uint8_t dpl: 2;
        uint8_t seg_p: 1;
        uint8_t reserved: 4;
        uint8_t avl: 1;
        uint8_t r_mode: 1;
        uint8_t dos: 1;
        uint8_t g: 1;
        uint8_t seg_unusable: 1;
        uint16_t reserved: 15;
    };
} __attribute__((packed));

struct vmcs_component_encoding_t {
    uint8_t access: 1;
    uint16_t index: 9;
    uint8_t type: 2;
    uint8_t reserved1: 1;
    uint8_t width: 2;
    uint32_t reserved2: 17;
} __attribute__((packed));

/* VMCS Guest State Area */
struct vmcs_gstate_t {
    /* Register State */
    uint64_t cr0, cr3, cr4;
    uint64_t dr7;
    uint64_t rsp, rip, rflags;

    struct vmcs_selectors_t selectors[8];

    struct vmcs_segment_t gdtr;
    struct vmcs_segment_t idtr;

    /* Guest MSRS */
    uint64_t ia32_debugctl;
    uint32_t ia32_sysenter_cs;
    uint32_t ia32_sysenter_esp;
    uint32_t ia32_sysenter_eip;
    uint64_t ia32_perf_global_ctrl;
    uint64_t ia32_pat;
    uint64_t ia32_efer;
    uint64_t ia32_bndcfgs;
    uint64_t ia32_rtit_ctl;
    uint64_t ia32_s_cet;
    uint64_t ia32_interrupt_ssp_table_addr;
    uint64_t ia32_pkrs;

    /* Non-Register State */
    uint32_t state;

    struct int_state_t {
        uint8_t bsti: 1;
        uint8_t bmovss: 1;
        uint8_t bsmi: 1;
        uint8_t bnmi: 1;
        uint8_t encint: 1;
        uint32_t reserved: 27;
    };

    struct pdbg_t {
        uint8_t bcond: 4;
        uint8_t reserved1;
        uint8_t eb: 1;
        uint8_t reserved2: 1;
        uint8_t bs: 1;
        uint8_t reserved3: 1;
        uint8_t rtm: 1;
        uint64_t reserved4: 47;
    };

    uint64_t vmcs_link_ptr;
    uint32_t vmx_preempt_timer;
    uint64_t pdptes[4];
    uint8_t rvi;
    uint8_t svi;
    uint16_t pml_idx;
} __attribute__((packed));

/* VMCS Host State Area */
struct vmcs_hstate_t {
    uint64_t cr0, cr3, cr4;
    uint64_t rsp, rip;

    struct vmcs_selectors_t selectors[7];
    uint64_t fs, gs, tr, gdtr, idtr;

    /* Host MSRs */
    uint32_t ia32_sysenter_cs;
    uint32_t ia32_sysenter_esp;
    uint32_t ia32_sysenter_eip;
    uint64_t ia32_perf_global_ctrl;
    uint64_t ia32_pat;
    uint64_t ia32_efer;
    uint64_t ia32_s_cet;
    uint64_t ia32_interrupt_ssp_table_addr;
    uint64_t ia32_pkrs;

    uint64_t ssp_ptr_reg;
} __attribute__((packed));

struct vmexec_ctrl_t {
    uint32_t pinbvmexec_ctrls;

    uint32_t procbvmexec_ctrls;

    uint32_t exception_bitmap;

    uint64_t io_bitmaps[2];

    uint64_t tsc_offset;

    uint64_t cr0_guest_mask;
    uint64_t cr0_host_mask;
    uint64_t cr0_read_shadow;
    uint64_t cr4_guest_mask;
    uint64_t cr4_host_mask;
    uint64_t cr4_read_shadow;

    uint64_t cr3_target_vals[4];
    uint64_t cr3_target_cnt;

    uint64_t apic_paddr;
    uint64_t apic_vaddr;
    uint32_t tpr_thresh;
    uint64_t eoi_ebitmap[4];
    uint16_t pint_notif_vec;
    uint64_t pint_desc_addr;

    uint64_t read_bitmap_paddr[2];
    uint64_t write_bitmap_paddr[2];

    uint64_t evmcs_ptr;

    uint64_t eptp;

    uint16_t vpid;

    uint32_t ple_gap;
    uint32_t ple_window;

    uint64_t vm_func_ctrl;

    uint64_t vmread_bitmap_paddr;
    uint64_t vmwrite_bitmap_paddr;

    uint64_t encls_exiting_bitmap;
    uint64_t enclv_exiting_bitmap;

    uint64_t pml_log;

    uint64_t virt_except_info_addr;
    uint16_t eptp_idx;

    uint64_t xss_exiting_bitmap;

    // Sub Page Permission Table Pointer
} __attribute__((packed));

#define VMX_INT_EXT         0
#define VMX_INT_RESERVED    1
#define VMX_INT_NMI         2
#define VMX_INT_HARDEXCEPT  3
#define VMX_INT_SOFTINT     4
#define VMX_INT_PSOFTEXCEPT 5
#define VMX_INT_SOFTEXCEPT  6
#define VMX_INT_OTHER       7

struct vmexit_ctrl_t {
    uint32_t basic_ops;
    uint32_t vmexit_msr_sto_cnt;
    uint64_t vmexit_msr_sto_paddr;
    uint32_t vmexit_msr_lod_cnt;
    uint64_t vmexit_msr_lod_paddr;
} __attribute__((packed));

struct vmentry_ctrl_t {
    uint32_t vmentry_ctrls;
    uint32_t vmentry_msr_lod_cnt;
    uint64_t vmentry_msr_lod_paddr;
    uint32_t vmentry_int_info;
    uint32_t vmentry_except_code;
    uint32_t vmentry_ins_len;
} __attribute__((packed));

struct vmexit_info_t {
    uint32_t reason;
    uint64_t qualification;
    uint64_t g_lin_addr;
    uint64_t g_paddr;

    uint32_t vmexit_int_info;
    uint32_t vmexit_int_ecode;

    uint32_t idt_vec_info;
    uint32_t idt_vec_errcode;

    uint32_t vmexit_ins_len;
    uint32_t vmexit_ins_info;

    uint64_t rcs, rsi, rdi, rip;

    uint32_t vm_ins_err;
} __attribute__((packed));

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
    
}

static int vmxon() {
    void* vmxon_region = kmalloc(PAGESIZE);
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
        TRACE("entered vmxon operation\n");
    } else {
        ERR("vmxon operation failed!\n");
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

int init_vmx() {
    if (!has_vmx()) {
        ERR("cpu does not support vmx!");
        return 0;
    }

    TRACE("cpu supports vmx\n");

    if (!vmxon()) {
        goto error;
    }



    return 1;

    error:
        return 0;
}
