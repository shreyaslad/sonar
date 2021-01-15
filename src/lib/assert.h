#ifndef __ASSERT_H__
#define __ASSERT_H__

#undef __MODULE__
#define __MODULE__ "assert"

#include <trace.h>

#define ASSERT(expr) \
    if (!(expr)) { \
        ERR("Assertation \"%s\" failed\n", #expr) \
        ERR("\t%s:%d\n", __FILE__, __LINE__); \
        for (;;) ; \ 
    } \

#endif
