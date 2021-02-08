#include <sys/interrupts.h>

static char* exceptions[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void isr_handler(struct regs_t* regs) {
    #undef __MODULE__
    #define __MODULE__ "err"

    int_handler_t handler = get_handlers()[regs->int_no];

    if (regs->int_no >= 32) {
        if (handler) {
            handler(regs);
        } else {
            panic("No %s (int %#x) handler!",
                  exceptions[regs->int_no],
                  regs->int_no);
        }
    } else {
        asm volatile("cli");

        size_t cr2 = 0;
        asm volatile("mov %%cr2, %0"
                    : 
                    "=a"(cr2));

        size_t off;
        char* name = trace_addr(&off, regs->rip);

        ERR("%s on CPU #%u (int %#lx, <%s+%#lx>, e=%u)\n",
            exceptions[regs->int_no],
            get_cpu()->lapic_id,
            regs->int_no,
            name, off,
            regs->err_code);

        ERR("\trax: %#18lx     r8:  %#18lx\n",
            regs->rax,
            regs->r8);
        ERR("\trbx: %#18lx     r9:  %#18lx\n",
            regs->rbx,
            regs->r9);
        ERR("\trcx: %#18lx     r10: %#18lx\n",
            regs->rcx,
            regs->r10);
        ERR("\trdx: %#18lx     r11: %#18lx\n",
            regs->rdx,
            regs->r11);
        ERR("\trsp: %#18lx     r12: %#18lx\n",
            regs->rsp,
            regs->r12);
        ERR("\trbp: %#18lx     r13: %#18lx\n",
            regs->rbp,
            regs->r13);
        ERR("\trsi: %#18lx     r14: %#18lx\n",
            regs->rsi,
            regs->r14);
        ERR("\trdi: %#18lx     r15: %#18lx\n",
            regs->rdi,
            regs->r15);
        ERR("\trip: %#18lx     cr2: %#18lx\n",
            regs->rip,
            cr2);

        stacktrace(regs->rbp);
        asm volatile("hlt");
    }

    lapic_write(LAPIC_REG_EOI, 0); // EOI
}

