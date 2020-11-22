#ifndef __VEC_H__
#define __VEC_H__

#include <stdint.h>
#include <stddef.h>
#include <alloc.h>
#include <locks.h>

struct vector_t {
    void** items;
    size_t n;

    spinlock_t lock;
};

int vec_rmi(struct vector_t* v, void* item);
int vec_rm(struct vector_t* v, size_t idx);
void* vec_g(struct vector_t* v, size_t idx);
int vec_i(struct vector_t* v, void* item, size_t idx);
int vec_a(struct vector_t* v, void* item);
int vec_n(struct vector_t* v);

#endif
