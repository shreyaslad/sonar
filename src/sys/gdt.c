#include <sys/gdt.h>

#define N_ENTRIES 3
uint64_t gdt[N_ENTRIES];
size_t i = 0;

void load_gdt(struct gdt_ptr_t* ptr) {
    asm {
        lgdt [rdi]
    }
}

void store_gdt(struct gdt_ptr_t* ret) {
    asm {
        sgdt [rdi]
    }
}

void set_gdt() {
    struct gdt_ptr_t ptr = {
        .size = (N_ENTRIES * sizeof(uint64_t)) - 1,
        .table = (uint64_t)&gdt,
    };

    load_gdt(&ptr);
}

void init_gdt() {
    gdt[i++] = 0;
    gdt[i++] = (1ull << 43) | (1ull << 44) | (1ull << 47) | (1ull << 53); // Kernel Code
    gdt[i++] = (1ull << 41ull) | (1ull << 44) | (1ull << 47); // Kernel Data

    set_gdt();
}
