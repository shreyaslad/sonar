#ifndef __PANIC_H__
#define __PANIC_H__

#include <stdarg.h>
#include <io.h>
#include <sys/regs.h>

__attribute__((noreturn))
void panic(const char* reason, ...);

#endif
