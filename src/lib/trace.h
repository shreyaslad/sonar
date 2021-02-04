
#ifndef __TRACE_H__
#define __TRACE_H__

#include <stddef.h>
#include <log.h>
#include <sys/symlist.h>

struct frame_t {
    struct frame_t* rbp;
    size_t rip;
};

char* trace_addr(size_t* offset, size_t addr);
void stacktrace(size_t* rbp);

#endif
