#include <drivers/vesa.h>

struct stivale2_struct_tag_framebuffer* fb_info;

#define RED_SHIFT   16
#define GREEN_SHIFT 8
#define BLUE_SHIFT  0

struct color_t bg = {52, 84, 94};
struct color_t fg = {186, 186, 186};

size_t curx = 5;
size_t cury = 5;

static uint8_t* font;

uint32_t get_color(struct color_t* color) {
    return (uint32_t)((color->r << RED_SHIFT) | (color->g << GREEN_SHIFT) | (color->b << BLUE_SHIFT));
}

void plot_px(size_t x, size_t y, uint32_t color) {
    size_t fb_i = x + (fb_info->framebuffer_pitch / sizeof(uint32_t)) * y;
    uint32_t* fb = (uint32_t *)fb_info->framebuffer_addr;

    fb[fb_i] = color;
}

void clear_screen(struct color_t* color) {
    if (!color) {
        color = &bg;
    }
    
    for (int i = 0; i < fb_info->framebuffer_width; i++) {
        for (int j = 0; j < fb_info->framebuffer_height; j++) {
            plot_px(i, j, get_color(color));
        }
    }
}

void plot_char(char c, size_t x, size_t y, struct color_t* fg, struct color_t* bg) {
    int orig_x = x;
    uint8_t *glyph = &bitmap_font[c * BITMAP_FONT_HEIGHT];

    for (int i = 0; i < BITMAP_FONT_HEIGHT; i++) {
        for (int j = BITMAP_FONT_WIDTH - 1; j >= 0; j--)
            plot_px(x++, y, bit_test(glyph[i], j) ? get_color(fg) : get_color(bg));
        y++;
        x = orig_x;
    }

}

void put(char c) {
    if (c == '\n') {
        curx = 5;
        cury += 8;
    } else {
        plot_char(c, curx, cury, &fg, &bg);
        curx += 8;

        if (curx >= fb_info->framebuffer_width) {
            curx = 5;
            cury = 5;
        }
    }

    // TODO: implement scrolling
}

void puts(char* s) {
    while (*s) {
        put(*s++);
    }
}

void init_vesa(struct stivale2_struct_tag_framebuffer* fb) {
    fb_info = fb;
    fb_info->framebuffer_addr += HIGH_VMA;

    size_t fb_size = fb->framebuffer_height * fb->framebuffer_pitch;

    for (int i = fb->framebuffer_addr; i < fb->framebuffer_addr + fb_size; i += PAGESIZE) {
        vmm_map(i + HIGH_VMA, i, get_pml4(), TABLEPRESENT | TABLEWRITE);
    }

    TRACE("Found a %lux%lu display\n", fb->framebuffer_width, fb->framebuffer_height);
}
