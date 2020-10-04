#ifndef __PROC__REGS_H__
#define __PROC__REGS_H__

#define write_cr(reg, val) ({ \
    asm volatile ("mov cr" reg ", %0" : : "r" (val)); \
})

#define read_cr(reg) ({ \
    size_t cr; \
    asm volatile ("mov %0, cr" reg : "=r" (cr)); \
    cr; \
})

struct regs_t {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8, rsi, rdi, rbp, rdx, rcx, rbx,
        rax;
    uint64_t int_no, err_code;
    uint64_t rip, cs, rflags, rsp, ss;
} __attribute__((packed));

#endif
