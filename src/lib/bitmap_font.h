#ifndef __LIB__BITMAP_FONT_H__
#define __LIB__BITMAP_FONT_H__

#include <stdint.h>

#define BITMAP_FONT_WIDTH  8
#define BITMAP_FONT_HEIGHT 16
#define BITMAP_FONT_GLYPHS 256
#define BITMAP_FONT_MAX    (BITMAP_FONT_HEIGHT * BITMAP_FONT_GLYPHS)

extern uint8_t bitmap_font[BITMAP_FONT_MAX];

#endif
