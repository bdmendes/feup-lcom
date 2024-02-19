#pragma once
#include <lcom/lcf.h>

#define VBE_FUNCTION 0x4F
#define VBE_SET_MODE 0x02
#define VBE_VIDEO_SERVICE 0x10

#define VBE_MODE_1024x768 0x105
#define VBE_MODE_800x600 0x103
#define VBE_MODE_640x480 0x101

#define VBE_GET_MODE_INFO 0x01

int set_video_mode(uint16_t mode);
int map_graphics_vram(uint16_t mode);
int draw_pixel(uint16_t x, uint16_t y, uint32_t color);
