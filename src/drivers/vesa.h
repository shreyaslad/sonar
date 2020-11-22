#ifndef __DRIVERS__VESA_H__
#define __DRIVERS__VESA_H__

#include <stdint.h>
#include <stddef.h>
#include <trace.h>
#include <protos/stivale2.h>
#include <mem.h>
#include <mm/vmm.h>
#include <bit.h>

#undef __MODULE__
#define __MODULE__ "vesa"

struct color_t {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

extern struct stivale2_struct_tag_framebuffer* fb_info;

uint32_t get_color(struct color_t* color);

void plot_px(size_t x, size_t y, uint32_t color);
void plot_char(char c, size_t x, size_t y, struct color_t* fg, struct color_t* bg);
void put(char c);
void puts(char* s);
void clear_screen(struct color_t* color);

void init_vesa(struct stivale2_struct_tag_framebuffer* fb);

#endif