#ifndef __SYS__GDT_H__
#define __SYS__GDT_H__

#include <stdint.h>
#include <stddef.h>

struct gdt_ptr_t {
    uint16_t size;
    uint64_t table;
} __attribute__((packed));

void load_gdt(struct gdt_ptr_t* ptr);
void store_gdt(struct gdt_ptr_t* ret);

void set_gdt();

void init_gdt();

#endif
