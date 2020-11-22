[global vmread]
[global vmwrite]

[extern vmx_exit]

%include "sys/regs.asm"

section .text

vmx_exit_stub:
    cli
    pushaq

    call vmx_exit

    popaq
    sti
    vmresume