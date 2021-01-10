#include "font.h"

/**
 * @brief  draws a tile
 * @param  font: the font
 * @param  index: the index of the tile
 * @param  x: the x coordinate of the position
 * @param  y: the y coordinate of the position
 * @param  zoom: the zoom factor
 * @retval None
 */
static void(draw_tile)(Font *font, uint16_t index, uint16_t x, uint16_t y, bool zoom);

/**
 * @brief  gets the index of a given char
 * @param  c: the char
 * @retval the char's corresponding index in the font
 */
static uint8_t(get_char_index)(const char c);

static const char letters[] =
{'~', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-',
 '+', '!', '~', '#', '$', '%', '^', '&', '*', '(', ')', '_',
 '=', '{', '}', '[', ']', '|', '~', ':', ';', '"', '"', '<',
 ',', '>', '.', '?', '/', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S',
 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e',
 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q',
 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

Font* (create_font)(uint32_t tile_size, const char** font_map) {
  Font *font = (Font *) malloc(sizeof(Font));
  if (font == NULL) return NULL;
  font->tile_size = tile_size;

  /* create temp big sprite to be copied from */
  Sprite* full_font = create_sprite(font_map, 0, 0);
  if (full_font == NULL) return NULL;
  font->number_of_tiles = (full_font->height / font->tile_size) * (full_font->width / font->tile_size);

  /* assemble letter tiles */
  font->tiles = (Sprite **) malloc(font->number_of_tiles * sizeof(Sprite *));
  font->zoom_tiles = (Sprite **) malloc(font->number_of_tiles * sizeof(Sprite *));
  if (font->tiles == NULL || font->zoom_tiles == NULL) return NULL;
  uint8_t bytes = get_bytes_per_pixel();

  for (uint16_t index = 0; index < font->number_of_tiles; index++) {
    int xpos = index % (full_font->width / tile_size);
    int ypos = index / (full_font->width / tile_size);
    char* letter_map = (char *) malloc(bytes * font->tile_size * font->tile_size * sizeof(char));
    //if (letter_map == NULL) return NULL;

    for (uint16_t letter_row = 0; letter_row < tile_size; ++letter_row) {
      memcpy(letter_map + bytes * (letter_row * tile_size),
      full_font->map + bytes * (xpos * font->tile_size + (letter_row + ypos * font->tile_size) * full_font->width), bytes * font->tile_size);
    }

    font->tiles[index] = create_sprite_from_bitmap(letter_map, font->tile_size, font->tile_size, 0, 0);
    font->zoom_tiles[index] = create_cloned_sprite(font->tiles[index], 2);
    if (font->tiles[index] == NULL || font->zoom_tiles[index] == NULL) return NULL;
  }

  destroy_sprite(full_font);
  return font;
}

void (destroy_font)(Font *font) {
  if (font == NULL || font->tiles == NULL || font->zoom_tiles == NULL) return;
  for (uint16_t i = 0; i < font->number_of_tiles; i++) {
    destroy_sprite(font->tiles[i]);
    free(font->tiles[i]);
    font->tiles[i] = NULL;
    destroy_sprite(font->zoom_tiles[i]);
    free(font->zoom_tiles[i]);
    font->zoom_tiles[i] = NULL;
  }
  free(font->tiles);
  font->tiles = NULL;
  free(font->zoom_tiles);
  font->zoom_tiles = NULL;
}

static void (draw_tile)(Font *font, uint16_t index, uint16_t x, uint16_t y, bool zoom) {
  Sprite *tile = zoom ? font->zoom_tiles[index] : font->tiles[index];
  tile->x = x;
  tile->y = y;
  draw_sprite(get_current_video_buf(), tile);
}

static uint8_t (get_char_index)(const char c) {
  if (c == ' ') return 167; // last empty char in xpm
  for (uint32_t i = 0; i < sizeof(letters); ++i){
    if (letters[i] == c) return i;
  }
  return 0;
}

void (draw_string)(Font *font, const char *str, uint16_t x, uint16_t y, bool zoom) {
  uint16_t curr_x = x;
  for (uint8_t i = 0; str[i] != '\0'; ++i) {
    draw_tile(font, get_char_index(str[i]), curr_x, y, zoom);
    curr_x += zoom ? font->tile_size*2 : font->tile_size;
  }
}

void (draw_string_centered)(Font* font, const char* str, uint16_t y, bool zoom){
  uint16_t curr_x = get_mode_info()->XResolution/2;
  curr_x -= strlen(str)*font->tile_size*(zoom ? 2 : 1)/2;
  if (curr_x >= get_mode_info()->XResolution) curr_x = 0;
  draw_string(font, str, curr_x, y, zoom);
}
