#ifndef GRAPHICS_H
#define GRAPHICS_H

#define TRANSPARENT_COLOR 0x0

#include "vbe.h"

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color, bool allow_sobreposition);
int (vg_draw_matrix)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step);
int (vg_draw_pixmap)(uint8_t* sprite, uint16_t x, uint16_t y, xpm_image_t img);
int (vg_clear_screen)();
uint8_t get_red(uint32_t color);
uint8_t get_green(uint32_t color);
uint8_t get_blue(uint32_t color);

#endif
