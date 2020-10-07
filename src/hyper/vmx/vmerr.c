#include <hyper/vmx/vmerr.h>

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
#define VCMS_EXIT_APICACCES 44
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

struct vmcs_exit_t {
    uint16_t e_reason;
    uint8_t zero: 1;
    uint8_t undefined1;
    uint8_t svm: 1;
    uint8_t pending_mtf_vm_exit: 1;
    uint8_t e_root_op: 1;
    uint8_t undefined2: 1;
    uint8_t e_entry: 1;
} __attribute__((packed));

struct vmcs_vecinf_t {
    uint8_t vec;
    uint8_t type: 3;
    uint8_t e_code_valid: 1;
    uint8_t nmi_unblocking: 1;
    uint32_t undefined: 28;
    uint8_t valid: 1;
} __attribute__((packed));

void vmx_exit() {
    ;
}
