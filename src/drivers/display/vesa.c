#include <drivers/display/vesa.h>

struct stivale2_struct_tag_framebuffer* fb_info;

#define RED_SHIFT   16
#define GREEN_SHIFT 8
#define BLUE_SHIFT  0

static struct color_t bg = {0, 0, 0};
static struct color_t fg = {186, 186, 186};

static uint32_t curx, cury;

uint32_t get_color(struct color_t* color) {
    return (uint32_t)((color->r << RED_SHIFT) | (color->g << GREEN_SHIFT) | (color->b << BLUE_SHIFT));
}

void plot_px(uint32_t x, uint32_t y, uint32_t color) {
    uint32_t fb_i = x + (fb_info->framebuffer_pitch / sizeof(uint32_t)) * y;
    uint32_t* fb = (uint32_t *)(fb_info->framebuffer_addr);

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

void vesa_scroll() {
    uint32_t* fb = (uint32_t *)(fb_info->framebuffer_addr);

    uint64_t fb_height = fb_info->framebuffer_height;
    uint64_t fb_pitch = fb_info->framebuffer_pitch;

    uint64_t screen_height = fb_height / FONT_HEIGHT;
    uint64_t screen_pitch = fb_pitch / 4;

    for (uint64_t i = 1; i < screen_height; i++) {
        uint32_t* dest = fb + ((i - 1) * screen_pitch * FONT_HEIGHT);
        uint32_t* src = fb + (i * screen_pitch * FONT_HEIGHT);
        memcpy(dest, src, fb_pitch * FONT_HEIGHT);
    }

    memset(fb + ((screen_height - 1) * screen_pitch * FONT_HEIGHT), 0, fb_pitch * FONT_HEIGHT);

    curx = 0;
    cury = screen_height;
}

void plot_char(char c, uint32_t x, uint32_t y, struct color_t* fg, struct color_t* bg) {
    uint32_t fg_color = get_color(fg);
    uint32_t bg_color = get_color(bg);

    uint32_t* fb = (uint32_t *)(fb_info->framebuffer_addr);

    uint32_t* line = fb + y * FONT_HEIGHT * (fb_info->framebuffer_pitch / 4) + x * FONT_WIDTH;

    for (size_t i = 0; i < FONT_HEIGHT; i++) {
        uint32_t* dest = line;
        char dc = (c >= 32 && c <= 127) ? c : 127;
        uint32_t fontbits = font_bitmap[(dc - 32) * FONT_HEIGHT + i];

        for (size_t j = 0; j < FONT_WIDTH; j++) {
            int bit = (1 << ((FONT_WIDTH - 1) - j));
            *dest++ = (fontbits & bit) ? fg_color : bg_color;
        }

        line += (fb_info->framebuffer_pitch / 4);
    }
}

void put(char c) {
    if (c == '\n') {
        curx = 0;
        cury++;

        if (cury >= (fb_info->framebuffer_height / FONT_HEIGHT)) {
            vesa_scroll();
            cury--;
        }
    } else if (c == '\t') {
        curx += 3;
    } else {
        plot_char(c, curx, cury, &fg, &bg);
        curx++;

        if (curx >= (fb_info->framebuffer_width / FONT_WIDTH)) {
            curx = 0;
            cury++;

            if (cury >= (fb_info->framebuffer_height / FONT_HEIGHT)) {
                vesa_scroll();
                cury--;
            }
        }
    }
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
        vmm_map(i + HIGH_VMA, i, get_pml4(), MAP_TABLEPRESENT | MAP_TABLEWRITE);
    }
}
