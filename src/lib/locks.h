#ifndef __LOCKS_H__
#define __LOCKS_H__

#include <stdint.h>

typedef volatile uint64_t spinlock_t;

void spinlock_lock(spinlock_t* spinlock);
void spinlock_release(spinlock_t* spinlock);

#endif
