#include <lib/bitmap.h>

/* Helper Functions */

static uint64_t get_bitmap_entry(struct bitmap* bitmap, uint64_t abs_pos) {
    return bitmap->bitmap[abs_pos / PAGE_SIZE];
}

static uint8_t get_abs_bit(struct bitmap* bitmap, uint64_t abs_pos) {
    uint64_t entry = get_bitmap_entry(bitmap, abs_pos);
    uint64_t mask = (1 << (abs_pos % 64));

    return (bitmap->bitmap[entry] & mask) == mask;
}

static void set_abs_bit(struct bitmap* bitmap, uint64_t abs_pos) {
    uint64_t entry = get_bitmap_entry(bitmap, abs_pos);

    bitmap->bitmap[entry] |= (1 << (abs_pos % 64));
}

static void clear_abs_bit(struct bitmap* bitmap, uint64_t abs_pos) {
    uint64_t entry = get_bitmap_entry(bitmap, abs_pos);

    bitmap->bitmap[entry] &= ~(1 << (abs_pos % 64));
}

/* Allocation Functions */

uint64_t find_free_block(struct bitmap* bitmap, size_t n) {
    uint64_t start_pos = 0;
    bool block_found = false;

    uint64_t cur_bits_found = 0;

    // Loop through every possible page instead of
    // dealing with bitmap entries and offsets
    for (uint64_t i = 0; i < bitmap->len * 64; i++) {
        
        // The efficiency of doing this might be outweighed
        // by the inefficiency of running this calculation
        // on every bit
        // uint64_t entry = get_bitmap_entry(bitmap, i);

        // if (entry == 0xff) {
        //     i += 64;
        //     continue;
        // }

        if (!get_abs_bit(bitmap, i)) {
            cur_bits_found = 0;
            continue;
        }

        cur_bits_found++;

        if (cur_bits_found == n) {
            block_found = true;
            start_pos = i;

            break;
        }
    }

    // TODO: Panic if no free blocks found
    if (!block_found);

    return start_pos;
}

void bitmap_alloc(struct bitmap* bitmap, size_t abs_start, size_t n) {
    for (size_t i = abs_start; i < abs_start + n; i++) {
        set_abs_bit(bitmap, i);
    }
}


void bitmap_free(struct bitmap* bitmap, uint64_t abs_pos, size_t n) {
    for (size_t i = 0; i < n; i++) {
        clear_abs_bit(bitmap, abs_pos + i);
    }
}
