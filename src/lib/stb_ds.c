#include <alloc.h>

#define STB_DS_IMPLEMENTATION
#define STBDS_NO_SHORT_NAMES

#define STBDS_REALLOC(c, p, r) krealloc(p ,r)
#define STBDS_FREE(p, r) kfree(p)

#include <stb_ds.h>
