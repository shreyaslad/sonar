#include <mm/mem.h>

void init_mem(struct limine_memmap_response* mmap) {
    LOG("Scanning memory map\n");

    for (uint64_t i = 0; i < mmap->entry_count; i++) {
        struct limine_memmap_entry* cur_entry = mmap->entries[i];

        LOG("-\t%#016lx - %#016lx (%lu KiB): ",
            cur_entry->base,
            cur_entry->base + cur_entry->length,
            cur_entry->length / 1024
        );

        switch (cur_entry->type) {
            case LIMINE_MEMMAP_USABLE:
                printf("usable\n");
                break;
            case LIMINE_MEMMAP_RESERVED:
                printf("reserved\n");
                break;
            case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
                printf("acpi reclaimable\n");
                break;
            case LIMINE_MEMMAP_ACPI_NVS:
                printf("acpi nvs\n");
                break;
            case LIMINE_MEMMAP_BAD_MEMORY:
                printf("bad memory\n");
                break;
            case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
                printf("bootloader reclaimable\n");
                break;
            case LIMINE_MEMMAP_KERNEL_AND_MODULES:
                printf("kernel and modules\n");
                break;
            case LIMINE_MEMMAP_FRAMEBUFFER:
                printf("framebuffer\n");
                break;
            default:
                printf("unknown\n");
        }
    }

    init_pmm(mmap);

    LOG("Memory:\n");
    LOG("\tTotal: %lu (MiB)\n", get_total_mem() / 1024 / 1024);
    LOG("\tUsable: %lu (MiB)\n", get_usable_mem() / 1024 / 1024);
}
