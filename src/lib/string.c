#include <lib/string.h>

#define BYTES_TO_MIB(b) b / 1048576

void* memset(void* ptr, int value, size_t num) {
    uint8_t* buf = (uint8_t *)ptr;

    for (size_t i = 0; i < num; i++)
        buf[i] = (uint8_t)value;

    return ptr;
}

int memcmp(const void* ptr1, const void* ptr2, size_t num) {
    const uint8_t* a = (const uint8_t *)ptr1;
    const uint8_t* b = (const uint8_t *)ptr2;

    for (size_t i = 0; i < num; i++) {
        if(a[i] < b[i])
            return -1;
        else if(b[i] < a[i])
            return 1;
    }

    return 0;
}

void* memcpy(void* dest, const void* src, size_t n) {
    uint8_t* _dst = (uint8_t *)dest;
    const uint8_t* _src = (const uint8_t *)src;

    for(size_t i = 0; i < n; i++)
        _dst[i] = _src[i];
    
    return dest;
}

void* memmove(void* dest, const void* src, size_t num) {
    uint8_t* pdest = (uint8_t *)dest;
    const uint8_t* psrc = (const uint8_t *)src;

    if (src > dest) {
        for (size_t i = 0; i < num; i++) {
            pdest[i] = psrc[i];
        }
    } else if (src < dest) {
        for (size_t i = num; i > 0; i--) {
            pdest[i - 1] = psrc[i - 1];
        }
    }

    return dest;
}