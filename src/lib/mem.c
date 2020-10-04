#include <lib/mem.h>

void* memset(void* bufptr, int value, size_t size) {
    unsigned char* buf = (unsigned char*)bufptr;
    for (uint64_t i = 0; i < size; i++) {
        buf[i] = value;
    }

    return bufptr;
}

void* memcpy(void* dest, const void* src, size_t size) {
    const unsigned char* src2 = (const unsigned char*)src;
    unsigned char* dst = (unsigned char*)dest;
    for (uint64_t i = 0; i < size; i++) {
        dst[i] = src2[i];
    }
    return dst;
}

int memcmp(const void* s1, const void* s2, size_t n) {
    const uint8_t* p1 = s1;
    const uint8_t* p2 = s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

void init_mem(struct stivale2_struct_tag_memmap* memmap) {
    TRACE("Bootstrapping Memory:\n");

    struct stivale2_mmap_entry* entry = (struct mmap_entry_t *)memmap->memmap;

    for (uint64_t i = 0; i < memmap->entries; i++) {
        totalmem += entry[i].length;
    }

    for (uint64_t i = 0; i < totalmem / PAGESIZE; i += PAGESIZE) {
        vmm_map((uint64_t *)(i + HIGH_VMA), (uint64_t *)i, get_pml4(), TABLEPRESENT | TABLEWRITE);
        vmm_map((uint64_t *)(i + KERNEL_HIGH_VMA), (uint64_t *)i, get_pml4(), TABLEPRESENT | TABLEWRITE);
        vmm_map((uint64_t *)i, (uint64_t *)i, get_pml4(), TABLEPRESENT | TABLEWRITE);
    }

    memset(pmm_bitmap, 0, totalmem / PAGESIZE / 8);

    for (uint64_t i = 0; i < memmap->entries; i++) {
        TRACE("\t%#016x - %#016x: ",
                entry[i].base,
                entry[i].base + entry[i].length);

        switch (entry[i].type) {
            case STIVALE2_MMAP_USABLE:
                printf("Usable\n");
                break;
            case STIVALE2_MMAP_RESERVED:
                printf("Reserved\n");
                break;
            case STIVALE2_MMAP_ACPI_RECLAIMABLE:
                printf("ACPI Reclaimable\n");
                break;
            case STIVALE2_MMAP_ACPI_NVS:
                printf("NVS\n");
                break;
            case STIVALE2_MMAP_BAD_MEMORY:
                printf("Bad RAM\n");
                break;
            case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE:
                printf("Limine Reclaimable\n");
                break;
            case STIVALE2_MMAP_KERNEL_AND_MODULES:
                printf("Kernel/Modules\n");
                break;
        }

        if (entry[i].type != STIVALE2_MMAP_USABLE) {
            set_abs_bit(pmm_bitmap, entry[i].base / PAGESIZE);
        }
    }

    TRACE("Available Memory: %uGiB\n",
            totalmem / 1073741824);

    return;
}
