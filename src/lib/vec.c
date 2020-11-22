#include <vec.h>

int vec_rmi(struct vector_t* v, void* item) {
    return -1;
}

int vec_rm(struct vector_t* v, size_t idx) {
    if (!v)
        return 0;
    
    spinlock_lock(&v->lock);

    if ((idx + 1) > v->n)
        return 0;

    v->items[idx] = NULL;

    for (size_t i = idx; i < v->n; i++) {
        v->items[i] = v->items[i + 1];
    }

    krealloc(v->items, v->n - 1);
    v->n--;

    spinlock_lock(&v->lock);
    return 1;
}

void* vec_g(struct vector_t* v, size_t idx) {
    if (!v)
        return 0;

    spinlock_lock(&v->lock);

    if ((idx + 1) > v->n)
        return NULL;

    spinlock_release(&v->lock);
    return v->items[idx];
}

int vec_i(struct vector_t* v, void* item, size_t idx) {
    if (!v)
        return 0;

    spinlock_lock(&v->lock);

    if ((idx + 1) > v->n)
        return 0;

    void** cpy = kmalloc(sizeof(void *) * v->n++);
    
    for (size_t i = 0; i < idx; i++) {
        cpy[i] = v->items[i];
    }

    cpy[idx] = item;

    for (size_t i = (idx + 1); i < (v->n - idx); i++) {
        cpy[i] = v->items[i];
    }

    kfree(v->items);
    v->items = cpy;

    spinlock_release(&v->lock);
    return 1;
}

int vec_a(struct vector_t* v, void* item) {
    if (!v)
        return 0;

    spinlock_lock(&v->lock);

    v->items = krealloc(v->items, v->n + 1);
    v->items[v->n] = item;
    v->n++;

    spinlock_release(&v->lock);
    return 1;
}

int vec_n(struct vector_t* v) {
    if (!v)
        return 0;

    v->lock = 0; // is this safe
    spinlock_lock(&v->lock);

    v->items = NULL;
    v->n = 0;

    spinlock_release(&v->lock);
    return 1;
}
