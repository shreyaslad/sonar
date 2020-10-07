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

[extern vmx_exit]

%include "src/sys/regs.asm"

vmx_exit_stub:
    cli
    pushaq

    call vmx_exit

    popaq
    sti
    vmresume