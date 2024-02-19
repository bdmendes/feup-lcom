#ifndef L5R_VBE_H
#define L5R_VBE_H

#include <lcom/lcf.h>

/* bios services */
#define BIOS_VIDEO_CARD 0x10
#define BIOS_PC_CONFIG 0x11
#define BIOS_MEM_CONFIG 0x12
#define BIOS_KEYBD_CONFIG 0x16

/* vbe 0x10 interface */
#define VBE_FUNCTION_AH 0x4F
#define VBE_SET_MODE 0x02
#define VBE_GET_MODE_INFO 0x01
#define VBE_LINEAR_FB BIT(14)
#define VBE_GRAPHIC_1024_768_256 0x105

/* function 01h mode attributes characteristics */
#define VBE_MODE_SUPPORTED BIT(0)
#define VBE_LINEAR_BUFFER_AVAILABLE BIT(7)

/* functions */
int vbe_set_mode(uint16_t mode);
int vbe_map_vram(uint16_t mode);
void* (vbe_get_video_mem)();
vbe_mode_info_t* (vbe_get_vbe_m_info)();
int (m_vbe_get_mode_info)(uint16_t mode, vbe_mode_info_t* vmi_p);

#endif
