#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <mm/common.h>

struct bitmap {
    uint64_t* bitmap;
    size_t len;
};

uint64_t find_free_block(struct bitmap* bitmap, size_t n);

void bitmap_alloc(struct bitmap* bitmap, size_t abs_start, size_t n);
void bitmap_free(struct bitmap* bitmap, uint64_t abs_pos, size_t n);

#endif
