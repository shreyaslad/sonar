#ifndef __MM__BIT_H__
#define __MM__BIT_H__

#include <stdint.h>
#include <stddef.h>

#define bit_test(var, offset) ({ \
    int __ret; \
    asm volatile ( \
        "bt %1, %2;" \
        : "=@ccc" (__ret) \
        : "r" ((uint32_t)(var)), "r" ((uint32_t)(offset)) \
    ); \
    __ret; \
})

uint8_t get_bit(uint8_t num, uint8_t bit);
void set_bit(uint8_t* num, uint8_t bit, uint8_t state);

uint8_t get_abs_bit(uint64_t* bitmap, uint64_t bit);
void set_abs_bit(uint64_t* bitmap, uint64_t bit);
void cls_abs_bit(uint64_t* bitmap, uint64_t bit);

#endif
