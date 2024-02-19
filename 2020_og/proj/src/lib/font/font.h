#ifndef FONT_H
#define FONT_H

#include "../sprite/sprite.h"

/**
 * @brief A Font is composed of each letter sprite, in its original size and zoomed 2x.
 */
typedef struct {
  Sprite **tiles;
  Sprite **zoom_tiles;
  uint16_t tile_size;
  uint32_t number_of_tiles;
} Font;

/**
 * @brief  allocates a font
 * @param  tile_size: the size of the tiles
 * @param  font_map: image containing all the letters by the order that is specified in the font translation unit and all with the same width.
 * @retval the font created
 */
Font *(create_font)(uint32_t tile_size, const char** font_map);

/**
 * @brief  deallocates a font
 * @param  font: the font to be destroyed
 * @retval None
 */
void(destroy_font)(Font *font);

/**
 * @brief  draws a string of tiles
 * @param  font: the font
 * @param  str: the string
 * @param  x: the x_coordinate of the starting position
 * @param  y: the y_coordinate of the starting position
 * @param  zoom: whether to zoom 2x or not
 * @retval 
 */
void(draw_string)(Font *font, const char *str, uint16_t x, uint16_t y, bool zoom);

/**
 * @brief  draws a string centered in the x axis, in the desired y position
 * @param  font: the font
 * @param  str: the string
 * @param  y: the y coordinate (counting from the top)
 * @param  zoom: the zoom factor
 * @retval 
 */
void(draw_string_centered)(Font *font, const char *str, uint16_t y, bool zoom);

#endif
