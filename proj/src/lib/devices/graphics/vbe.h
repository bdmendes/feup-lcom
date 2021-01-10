#ifndef VBE_H
#define VBE_H

#include <lcom/lcf.h>

/* BIOS services (interrupt number) */
#define BIOS_VIDEO_CARD 0x10
#define BIOS_PC_CONFIG 0x11
#define BIOS_MEM_CONFIG 0x12
#define BIOS_KEYBOARD 0x16

/* Graphic modes */
#define GRAPHICS_MODE_INDEXED 0x105
#define GRAPHICS_MODE_DIRECT_15BIT 0x115
#define GRAPHICS_MODE_DIRECT_32BIT 0x14C
#define GRAPHICS_LINEAR_MODE_BIT BIT(14)

/* VESA API */
#define AH_INPUT_FUNCTION 0x4F
#define AL_INPUT_SET_MODE 0x02
#define AL_INPUT_GET_MODE_INFO 0x01
#define AL_INPUT_SET_DISPLAY_START 0x07
#define BL_INPUT_SET_DISPLAY_START 0x00
#define BL_INPUT_SET_DISPLAY_START_DURING_VERTICAL_RETRACE 0x80
#define AH_OUTPUT_NO_ERRORS 0x0
#define AL_OUTPUT_FUNCTION_CALL_SUCCEEDED 0x4F

/* functions */

/**
 * @brief  changes the graphics card mode and maps the vram accordingly
 * @param  mode: the desired graphic mode
 * @retval OK if success, otherwise !OK
 */
int(vg_switch_mode)(uint16_t mode);

/**
 * @brief  gets the pointer to the video buffer that is being written to (which is in the moment hidden, until the next page flipping occurs)
 * @retval the video buffer
 */
char *(get_current_video_buf)();

/**
 * @brief  gets the current graphic mode information, that was retrieved when switching to the graphics mode
 * @retval the infotmation
 */
vbe_mode_info_t *(get_mode_info)();

/**
 * @brief  gets the number of bytes per pixel for the current graphics mode, that was retrieved when switching to the graphics mode
 * @retval the number of bytes per pixel
 */
uint8_t(get_bytes_per_pixel)();

/**
 * @brief  calls VBE function 0x07 to flip the page to the current video buffer. The current video buffer is then switched to be the next one of the three (wraps around after the third)
 * @retval OK if success, otherwise !OK
 */
int(vg_flip_page)();

/**
 * @brief  signals that new information is available to be drawn. vg_has_updates() will return true and allow the app state handler to know to execute 3 page flips with fresh drawings within the next 3 timer ticks.
 * @retval None
 */
void(vg_signal_update)();

/**
 * @brief  checks if there was a request to flip the video page to one with new information, that shall be drawn.
 * @retval true if there are updates, false otherwise
 */
bool (vg_has_updates)();

/**
 * @brief  draws a pixel to a given position in a given color. Transparency color for 0x14C mode is discarded and the function returns. Does not check for outbounds positions.
 * @param  buffer: the buffer where the pixel is drawn
 * @param  x: the x coordinate of the position
 * @param  y: the y coordinate of the position
 * @param  color: the color
 * @retval None
 */
void(draw_pixel)(char *buffer, uint16_t x, uint16_t y, uint32_t color);

#endif
