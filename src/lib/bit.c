#include <bit.h>

uint8_t get_bit(uint8_t num, uint8_t bit) {
    return ((num >> bit) & 1);
}

void set_bit(uint8_t* num, uint8_t bit, uint8_t state) {
    if (state == 0) {
        *num &= ~(1 << bit);
    } else if (state == 1) {
        *num = (*num | (1 << bit));
    }

    return;
}

uint8_t get_abs_bit(uint64_t* bitmap, uint64_t bit) {
    size_t off = bit / 64;
    size_t mask = (1 << (bit % 64));

    return (bitmap[off] & mask) == mask;
}

void set_abs_bit(uint64_t* bitmap, uint64_t bit) {
    size_t off = bit / 64;
    //size_t mask = (1 << (bit % 64));

    bitmap[off] = (bitmap[off] | (1 << bit));
}

void cls_abs_bit(uint64_t* bitmap, uint64_t bit) {
    size_t off = bit / 64;
    //size_t mask = (1 << (bit % 64));

    bitmap[off] &= ~(1 << bit);
}
