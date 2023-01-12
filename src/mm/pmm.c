#include <mm/pmm.h>

static volatile struct bitmap pmm_bitmap = {
    .bitmap = NULL,
    .len = 0
};

static size_t total_mem; // In bytes
static size_t usable_mem; // In bytes

size_t get_total_mem() {
    return total_mem;
}

size_t get_usable_mem() {
    return usable_mem;
}

int pmm_free(void* ptr, size_t n) {
    uint64_t page = (uint64_t)ptr / PAGE_SIZE;

    bitmap_free(&pmm_bitmap, page, n);

    return 1;
}

void* pmm_alloc(size_t n) {
    uint64_t free_block = find_free_block(&pmm_bitmap, n);
    bitmap_alloc(&pmm_bitmap, free_block, n);

    return (void *)free_block;
}

void* pmm_realloc(void* ptr, size_t old_size, size_t new_size) {
   void* ret = pmm_alloc(new_size);

   memcpy(ret, ptr, old_size * PAGE_SIZE);
   pmm_free(ptr, old_size);

   return ret;
}

void init_pmm(struct limine_memmap_response* mmap) {
    /**
     * 1. Loop through mmap, find total amount of memory
     * 2. Calculate bitmap size
     * 3. Find available portion to hold bitmap
     * 4. Set bitmap to that location
     * 5. Mark bitmap & unusuable entries as allocated
     */

    // 1st Pass: Find total memory
    for (uint64_t i = 0; i < mmap->entry_count; i++) {
        struct limine_memmap_entry* cur_entry = mmap->entries[i];

        total_mem += cur_entry->length;
    }

    pmm_bitmap.len = total_mem / PAGE_SIZE / sizeof(uint64_t);

    // 2nd Pass: Find suitable region of available memory,
    //  & set bitmap there
    for (uint64_t i = 0; i < mmap->entry_count; i++) {
        struct limine_memmap_entry* cur_entry = mmap->entries[i];

        if (cur_entry->type == LIMINE_MEMMAP_USABLE
         && cur_entry->length > pmm_bitmap.len * PAGE_SIZE) {
            pmm_bitmap.bitmap = (uint64_t *)(cur_entry->base);

            break;
        }
    }

    memset(pmm_bitmap.bitmap, 0, pmm_bitmap.len * sizeof(uint64_t));

    // 3rd Pass: Mark unusable memory as allocated
    for (uint64_t i = 0; i < mmap->entry_count; i++) {
        struct limine_memmap_entry* cur_entry = mmap->entries[i];

        if (cur_entry->type != LIMINE_MEMMAP_USABLE
         || cur_entry->type != LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE
         || cur_entry->type != LIMINE_MEMMAP_ACPI_RECLAIMABLE) {

            uint64_t current_page = cur_entry->base / PAGE_SIZE;
            size_t unusable_page_span = cur_entry->length / PAGE_SIZE;

            bitmap_alloc(&pmm_bitmap, current_page, unusable_page_span);
        } else {
            // Keep track of usable memory

            usable_mem += cur_entry->length;
        }
    }

    // 4th Pass: Mark bitmap as allocated
    uint64_t bitmap_page = (uint64_t)(pmm_bitmap.bitmap) / PAGE_SIZE;
    size_t bitmap_page_span = (pmm_bitmap.len * sizeof(uint64_t)) / PAGE_SIZE;

    bitmap_alloc(&pmm_bitmap, bitmap_page, bitmap_page_span);
}
