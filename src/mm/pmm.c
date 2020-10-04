#include <mm/pmm.h>

extern uint64_t __kernel_end;
volatile uint64_t* pmm_bitmap = (uint64_t *)&__kernel_end;

uint64_t totalmem;
uint64_t bitmapEntries;

void* pmm_alloc(size_t pages) {
    uint64_t first_bit = 0;
    uint64_t concurrent_bits = 0;
    uint64_t total_bits_in_bitmap = totalmem / PAGESIZE;

    for (uint64_t i = 1; i < total_bits_in_bitmap; i++) {

        if (get_abs_bit(pmm_bitmap, i) == 0) {
            if (concurrent_bits == 0) {
                first_bit = i;
            }

            concurrent_bits++;

            if (pages == concurrent_bits) {
                goto alloc;
            }
        } else {
            first_bit = 0;
            concurrent_bits = 0;

            continue;
        }
    }

    return NULL;

alloc:
    // iterate over bits now that a block has been found
    for (uint64_t i = first_bit; i < first_bit + pages; i++) {
        set_abs_bit(pmm_bitmap, i);
    }
    return (void*)(first_bit * PAGESIZE);
}

void pmm_free(void* ptr, size_t pages) {
    uint64_t first_bit = (uint64_t)ptr / PAGESIZE;
    uint64_t total_bits_in_bitmap = totalmem / PAGESIZE;

    for (uint64_t i = 0; i < total_bits_in_bitmap; i++) {
        if (i == first_bit) {
            for (uint64_t j = 0; j < pages; j++) {
                cls_abs_bit(pmm_bitmap, j);
            }
            goto done;
        }
    }

done:
    return;
}

void* pmm_realloc(void* ptr, size_t old, size_t new) {
    if (new < PAGESIZE)
        return ptr;

    uint64_t* new_buffer = (uint64_t*)pmm_alloc(new);
    memcpy(new_buffer, ptr, old);
    pmm_free(ptr, old);

    return new_buffer;
}
