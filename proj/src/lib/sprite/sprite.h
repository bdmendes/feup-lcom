#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "../../lib/devices/graphics/vbe.h"
#include <lcom/lcf.h>

/**
 * @brief  A Sprite holds the width, height, x and y positions and the bitmap of an object that shall be drawn on the screen. If x are y, negative mean that the top left of the sprite is indeed ouside of the screen, and only a part of the sprite is shown.
 */
typedef struct Sprite {
  int32_t x, y;
  uint16_t width, height;
  char *map;
} Sprite;

/**
 * @brief  allocates a sprite object
 * @param  pic: the sprite's bitmap
 * @param  x: the x coordinate of the sprite's position
 * @param  y: the y coordinate of the sprite's position
 * @retval the created sprite
 */
Sprite *(create_sprite)(const char *pic[], int32_t x, int32_t y);

/**
 * @brief  deallocates a sprite object
 * @param  fig: the sprite to destroy 
 * @retval None
 */
void(destroy_sprite)(Sprite *fig);

/**
 * @brief  draws a sprite to a buffer
 * @param  buffer: the desired buffer 
 * @param  sp: the desired sprite 
 * @retval OK if success, else !OK
 */
int(draw_sprite)(char *buffer, Sprite *sp);

/**
 * @brief  changes the sprite position attributes
 * @param  fig: the animated sprite
 * @param  xmov: the x axis displacement
 * @param  ymov: the y axis displacement
 * @param  allow_image_cut_left_up: whether to allow cut of the image's left and up sides or not (if not allowed and cuts, the sprite will not be moved)
 * @param  allow_image_cut_right_down: whether to allow cut of the image's right and down sides or not (if not allowed and cuts, the sprite will not be moved)
 * @retval OK if success, !OK otherwise
 */
int(move_sprite)(Sprite *fig, int16_t xmov, int16_t ymov, bool allow_image_cut_left_up, bool allow_image_cut_right_down);

/**
 * @brief  mirrors a pixmap
 * @param  pixmap: the pixmap
 * @param  width: the pixmap's width
 * @param  height: the pixmap's height
 * @retval the mirrored pixmap
 */
char *(get_mirrored_pixmap)(const char *pixmap, uint16_t width, uint16_t height);

/**
 * @brief  checks if a coordinate is within a sprite (in its current position)
 * @param  x: the x coordinate of a position
 * @param  y: the y coordinate of a position
 * @param  sp: the sprite
 * @retval true if the position is within the sprite, false otherwise
 */
bool(is_inside_sprite)(uint16_t x, uint16_t y, Sprite *sp);

/**
 * @brief  creates a sprite from a bitmap
 * @param  map: the pixmap
 * @param  width: the pixmap's width
 * @param  height: the pixmap's height
 * @param  x: the x coordinate of the sprite's position
 * @param  y: the y coordinate of the sprite's position
 * @retval the created sprite
 */
Sprite *(create_sprite_from_bitmap)(char *map, uint16_t width, uint16_t height, int32_t x, int32_t y);

/**
 * @brief  draws a sprite in the original size to a buffer and with the requested zoom factor in other.
 * @param  og_buf: the original buffer (where the sprite is drawn first)
 * @param  zoom_buf: the zoom buffer (to where the sprite is copied)
 * @param  sp: the sprite
 * @param  zoom_factor: the desired zoom factor
 * @retval None
 */
void(draw_sprite_zoom)(char *og_buf, char *zoom_buf, Sprite *sp, uint8_t zoom_factor);

/**
 * @brief  zooms the sprite bitmap (cloning pixels in matrix-like form) and return new generated sprite
 * @param  sp: the original sprite
 * @param  zoom_factor: the desired zoom factor
 * @retval the new sprite
 */
Sprite *(create_cloned_sprite)(Sprite *sp, uint8_t zoom_factor);

/**
 * @brief  cheap sprite draw; does not check for out of bound positions and does not support negative sprite positions
 * @param  sp: the sprite
 * @retval None
 */
void(draw_sprite_no_checks)(Sprite *sp);

#endif
