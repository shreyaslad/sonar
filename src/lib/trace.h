#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <io.h>
#include <sys/symlist.h>

// stolen from the tomat

#ifndef __MODULE__
    #define __MODULE__ "slate"
#endif

#define PRN(stat, fmt, ...) \
    printf(stat " %-5s: " fmt, __MODULE__, ## __VA_ARGS__)

#define TRACE(fmt, ...) PRN("[*]", fmt, ## __VA_ARGS__)
#define WARN(fmt, ...)  PRN("[-]", fmt, ## __VA_ARGS__)
#define ERR(fmt, ...)   PRN("[!]", fmt, ## __VA_ARGS__)

struct frame_t {
    struct frame_t* rbp;
    size_t rip;
};

char* trace_addr(size_t* offset, size_t addr);
void stacktrace(size_t* rbp);

#endif
