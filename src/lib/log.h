#ifndef LOG_H
#define LOG_H

#include <lib/io.h>

#define PRN(stat, fmt, ...) \
    printf(stat " %-6s: " fmt, __MODULE__, ## __VA_ARGS__)

#define LOG(fmt, ...) PRN("[*]", fmt, ## __VA_ARGS__)
#define WARN(fmt, ...) PRN("[-]", fmt, ## __VA_ARGS__)
#define ERR(fmt, ...) PRN("[!]", fmt, ## __VA_ARGS__)

#endif
