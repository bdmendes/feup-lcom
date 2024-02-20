#define VBE_GRAPHICS_INTNO 0x10
#define VBE_GRAPHICS_FUNCTION_AH 0x4f
#define VBE_SET_MODE_AL 0x02

#include <lcom/lcf.h>

void vg_set_mode(int mode);

void map_vram(int mode);

void draw_pixel(uint32_t color, uint16_t x, uint16_t y);