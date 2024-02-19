#ifndef _ASPRITE_H_
#define _ASPRITE_H_

#include "sprite.h"

/**
 * @brief  An AnimSprite is composed of a sprite which points to the current frame bitmap. The map is switched when it's time to show the next animation (according to the frames_per_map attribute). According to the direction of the animated sprite, it may be flipped horizontally.
 */
typedef struct {
  Sprite *cur_sp;
  uint8_t num_fig;
  uint8_t cur_fig;
  char **map;
  char **mirror_map;
  uint8_t frames_per_map;
  uint8_t frames_left;
  bool is_idle;
  bool should_mirror;
  bool *is_mirrored;
} AnimSprite;

/**
 * @brief  allocates an animated sprite object
 * @param  x: the x coordinate of the desired position of the animated sprite
 * @param  y: the y coordinate of the desired position of the animated sprite
 * @param  frames_per_map: the number of frames to show before switching to the
 * next picture
 * @param  no_pic: number of different pictures
 * @param  picture1[]: the first picture
 * @param  ...: the rest of the pictures
 * @retval the created animated sprite object
 */
AnimSprite *(create_asprite)(uint16_t x, uint16_t y, uint8_t frames_per_map,
                             uint8_t no_pic, const char *picture1[], ...);
/**
 * @brief  draws an animated sprite to a buffer
 * @param  buffer: the buffer where the sprite should be drawn
 * @param  fig: the animated sprite
 * @retval OK if success, else !OK
 */
int(draw_asprite)(char *buffer, AnimSprite *fig);

/**
 * @brief  makes the animated sprite show the default image, with attention to
 * whether it is mirrored or not
 * @param  fig: the animated sprite
 * @retval None
 */
void(asprite_go_idle)(AnimSprite *fig);

/**
 * @brief  switches to the next sprite frame, either the same bitmap (there are
 * frames left) or the next one
 * @param  fig: the animated sprite
 * @retval None
 */
void(animate_asprite)(AnimSprite *fig);

/**
 * @brief  deallocates an animated sprite: all animation bitmaps are destroyed and the so is the sprite holder.
 * @param  fig: the animated sprite to destroy
 * @retval None
 */
void(destroy_asprite)(AnimSprite *fig);

/**
 * @brief  moves the animated sprite with consideration to how it should be drawn
 * @param  fig: the animated sprite
 * @param  xmov: the x axis displacement
 * @param  ymov: the y axis displacement
 * @param  allow_image_cut_left_up: whether to allow the image's left and up sides cut or not (nevertheless, at least one pixel shall be in the screen)
 * @param  allow_image_cut_right_down: whether to allow image's right and down sides cut or not (never the less, at least one pixel shall be in the screen)
 * @retval
 */
int(asprite_move)(AnimSprite *fig, int16_t xmov, int16_t ymov,
                  bool allow_image_cut_left_up,
                  bool allow_image_cut_right_down);

#endif
