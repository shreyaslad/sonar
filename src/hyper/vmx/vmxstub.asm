[global vmread]
[global vmwrite]

vmxon:
    ; stub
    ret

vmread:
    vmread rdi, rax
    ret

vmwrite:
    vmwrite rdi, rsi
    ret

[extern vmm_exit_handler]

%include "src/sys/regs.asm"

vmm_exit_stub:
    cli
    pushaq

    call vmm_exit_handler

    popaq
    sti
    vmresume