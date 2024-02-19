#ifndef L5R_GRAPHICS_H
#define L5R_GRAPHICS_H

#include <lcom/lcf.h>
#include "vbe.h"
#include "my_utils.h"
#include "lcom/pixmap.h"

/* functions */ 
int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color);
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
int (vg_draw_matrix)(uint8_t no_rectangles, uint32_t first, uint8_t step);
int (vg_draw_pixmap)(uint8_t* pixel_map, xpm_image_t* img, uint16_t x, uint16_t y);


#endif // L5R_GRAPHICS_H
