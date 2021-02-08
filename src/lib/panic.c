#include <panic.h>

__attribute__((noreturn))
void panic(const char* reason, ...) {
    va_list va;
    va_start(va, reason);
    char buffer[1];
    vsnprintf(buffer, (size_t)-1, reason, va);
    va_end(va);

    ERR("Kernel Panic!\n");
    ERR("Reason: %s\n", buffer);

    asm volatile("cli\n\thlt");
}
