[global vmread]
[global vmwrite]

%include "sys/regs.asm"

section .text

vmx_exit_stub:
    cli
    pushaq

    popaq
    sti
    vmresume