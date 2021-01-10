#ifndef VBE_H
#define VBE_H

#include <lcom/lcf.h>
#include "my_utils.h"

// BIOS services (interrupt number)
#define BIOS_VIDEO_CARD 0x10
#define BIOS_PC_CONFIG 0x11
#define BIOS_MEM_CONFIG 0x12
#define BIOS_KEYBOARD 0x16

// VBE modes
#define GRAPHICS_MODE_1024x768_INDEXED 0x105
#define GRAPHICS_MODE_640x480_DIRECT 0x110
#define GRAPHICS_MODE_800x600_DIRECT 0x115
#define GRAPHICS_MODE_1280x1024_DIRECT 0x11A
#define GRAPHICS_MODE_1152x864_DIRECT 0x14C

#define GRAPHICS_LINEAR_MODE_BIT BIT(14)

#define AH_FUNCTION 0x4F
#define AL_VBE_SET_MODE 0x02
#define AL_VBE_RETURN_MODE_INFO 0x01

// functions
int (set_graphics_mode)(uint16_t mode);
int (map_vram_memory)(uint16_t mode);
int (vbe_get_mode_info_ours)(uint16_t mode, vbe_mode_info_t* mode_info);
vbe_mode_info_t* (get_mode_info)();
void* (get_video_mem)();

#endif
