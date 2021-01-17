#ifndef __DRIVERS__VESA_H__
#define __DRIVERS__VESA_H__

#include <stdint.h>
#include <stddef.h>
#include <alloc.h>
#include <trace.h>
#include <protos/stivale2.h>
#include <mem.h>
#include <mm/vmm.h>
#include <bit.h>
#include <font.h>
#include <panic.h>

#define __MODULE__ "vesa"

struct color_t {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

uint32_t get_color(struct color_t* color);

void plot_px(uint32_t x, uint32_t y, uint32_t color);

void plot_char(char c, uint32_t x, uint32_t y, struct color_t* fg, struct color_t* bg);
void put(char c);

void puts(char* s);

void clear_screen(struct color_t* color);
void vesa_scroll();

void init_vesa(struct stivale2_struct_tag_framebuffer* fb);

#endif
