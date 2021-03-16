#include <lib/mem.h>

void* memset(void* s, int c, size_t n) {
    uint8_t* buf = (uint8_t*)s;

    for(size_t i = 0; i < n; i++)
        buf[i] = (uint8_t)c;

    return s;
}

int memcmp(const void* s1, const void* s2, size_t n) {
    const uint8_t* a = (const uint8_t*)s1;
    const uint8_t* b = (const uint8_t*)s2;

    for(size_t i = 0; i < n; i++) {
        if(a[i] < b[i])
            return -1;
        else if(b[i] < a[i])
            return 1;
    }

    return 0;
}

void* memcpy(void* dst, const void* src, size_t n) {
    uint8_t* _dst = (uint8_t*)dst;
    const uint8_t* _src = (const uint8_t*)src;

    for(size_t i = 0; i < n; i++)
        _dst[i] = _src[i];
    
    return dst;
}

void* memmove(void* dest, const void* src, size_t n) {
    uint8_t* pdest = dest;
    const uint8_t* psrc = src;

    if (src > dest) {
        for (size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if (src < dest) {
        for (size_t i = n; i > 0; i--) {
            pdest[i - 1] = psrc[i - 1];
        }
    }

    return dest;
}

void init_mem(struct stivale2_struct_tag_memmap* memmap) {
    LOG("Memory map:\n");

    struct stivale2_mmap_entry* entry = (struct stivale2_mmap_entry *)memmap->memmap;

    uint64_t usable_mem = 0;

    for (uint64_t i = 0; i < memmap->entries; i++) {
        totalmem += entry[i].length;
    }

    for (uint64_t i = 0; i < totalmem / PAGESIZE; i += PAGESIZE) {
        vmm_map((uint64_t *)(i + HIGH_VMA), (uint64_t *)i, get_pml4(), MAP_TABLEPRESENT | MAP_TABLEWRITE);
        vmm_map((uint64_t *)(i + KERNEL_HIGH_VMA), (uint64_t *)i, get_pml4(), MAP_TABLEPRESENT | MAP_TABLEWRITE);
        vmm_map((uint64_t *)i, (uint64_t *)i, get_pml4(), MAP_TABLEPRESENT | MAP_TABLEWRITE);
    }

    memset(pmm_bitmap, 0, totalmem / PAGESIZE / 8);

    for (uint64_t i = 0; i < memmap->entries; i++) {
        LOG("-\t%#08x - %#08x: ",
                entry[i].base,
                entry[i].base + entry[i].length);

        switch (entry[i].type) {
            case STIVALE2_MMAP_USABLE:
                printf("usable\n");
                usable_mem += entry[i].length;
                
                break;
            case STIVALE2_MMAP_RESERVED:
                printf("reserved\n");
                break;
            case STIVALE2_MMAP_ACPI_RECLAIMABLE:
                printf("acpi reclaimable\n");
                break;
            case STIVALE2_MMAP_ACPI_NVS:
                printf("nvs\n");
                break;
            case STIVALE2_MMAP_BAD_MEMORY:
                printf("bad ram\n");
                break;
            case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE:
                printf("limine reclaimable\n");
                break;
            case STIVALE2_MMAP_KERNEL_AND_MODULES:
                printf("kernel/modules\n");
                break;
        }

        if (entry[i].type != STIVALE2_MMAP_USABLE) {
            set_abs_bit(pmm_bitmap, entry[i].base / PAGESIZE);
        }
    }

    LOG("Detected %d MiB of usable memory\n", usable_mem / 1048576);
}
