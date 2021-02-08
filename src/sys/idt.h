#ifndef __SYS__IDT_H__
#define __SYS__IDT_H__

#include <stdint.h>
#include <stddef.h>
#include <sys/regs.h>

struct idt_ptr_t {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

typedef void (*int_handler_t)(struct regs_t*);

int_handler_t* get_handlers();

void set_entry(int idx, int_handler_t handler);
void register_handler(uint8_t int_no, int_handler_t handler);

void load_idt(struct idt_ptr_t* ptr);
void store_idt(struct idt_ptr_t* ret);

void set_idt();

void init_idt();

#endif
