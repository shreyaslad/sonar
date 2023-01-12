#include <stdint.h>
#include <stddef.h>
#include <protos/limine.h>

#include <drivers/serial.h>
#include <mm/pmm.h>

static volatile struct limine_memmap_request mmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

static volatile struct limine_framebuffer_request fb_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

static volatile struct limine_terminal_request terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0
};

__attribute__((noreturn))
void _start(void) {
    init_serial();

    init_pmm(mmap_request.response);

    for (;;) {
        __asm__("hlt");
    }
}
