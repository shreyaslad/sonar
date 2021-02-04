#ifndef __LOG_H__
#define __LOG_H__

#include <io.h>

#ifndef __MODULE__
    #define __MODULE__ "slate"
#endif

#define PRN(stat, fmt, ...) \
    printf(stat " %-6s: " fmt, __MODULE__, ## __VA_ARGS__)

#define TRACE(fmt, ...) PRN("[*]", fmt, ## __VA_ARGS__)
#define WARN(fmt, ...)  PRN("[-]", fmt, ## __VA_ARGS__)
#define ERR(fmt, ...)   PRN("[!]", fmt, ## __VA_ARGS__)

#endif
