#ifndef MAP_H
#define MAP_H

#include "../../lib/sprite/sprite.h"

/**
 * @brief  the different possible move directions
 */
typedef enum MOVE_DIRECTION { DOWN, UP, RIGHT, LEFT } MOVE_DIRECTION;

/* zoom factor correspondence to the levels */
#define EASY_ZOOM_FACTOR 3
#define MEDIUM_ZOOM_FACTOR 6
#define HARD_ZOOM_FACTOR 10

/**
 * @brief Different possible objects in the map
 */
typedef enum MAP_OBJECT {
  FREEWAY,
  ROCK,
  BOMB,
  CLUE,
  DOOR,
  DEFUSED_BOMB
} MAP_OBJECT;

/**
 * @brief A Map holds information about the blocks for each position, and the sprites corresponding to each one, in original size and zoomed with the corresponding zoom factor. Since the zoomed map moves, the relative position of the map top left is also stored.
 */
typedef struct Map {
  MAP_OBJECT *objects;
  uint32_t width;
  uint32_t height;
  uint32_t block_length;
  uint16_t no_blocks_row;
  uint16_t no_blocks_col;
  char *buf;

  /* zoom related */
  uint32_t cur_x_start;
  uint32_t cur_y_start;
  char *zoom_buf;
  uint8_t zoom_factor;

  /* to draw again later (defuse bomb) */
  Sprite **sprites;
  uint8_t no_block_types;
} Map;

/**
 * @brief Allocates a map
 * @retval The map
 */
Map *(create_map)();

/**
 * @brief Deallocates a map
 * @param map: The map to be destroyed
 * @retval None
 */
void(destroy_map)(Map *map);

/**
 * @brief Copies the current position of the zoom buffer to the video buffer
 * @param map: the map to be drawn
 * @retval None
 */
void(draw_map)(Map *map);

/**
 * @brief Moves the map, giving the sensation of a moving camera
 * @param map: the map to move
 * @param dir: the direction to move the map
 * @param step: how much to move the map
 * @retval OK if the map can move !OK if not
 */
int(move_map)(Map *map, MOVE_DIRECTION dir, int16_t step);

/**
 * @brief loads the map to the buffers according to the zoom
 *        factor specified
 * @param map: the map
 * @param zoom_factor: the zoom factor
 * @retval None
 */
void(load_map)(Map *map, uint8_t zoom_factor);

/**
 * @brief  defuses a bomb in a given position
 * @param  map: the map
 * @param  x: the x coordinate
 * @param  y: the y coordinate
 * @retval OK if the object was a bomb to defuse, else !OK
 */
int(map_defuse_bomb)(Map *map, uint32_t x, uint32_t y);

/**
 * @brief  reactivates all the bombs in the map
 * @param  map: the map 
 * @retval None
 */
void(reactivate_all_bombs)(Map *map);

/**
 * @brief  returns the object in a given position
 * @param  x: the x coordinate of the position
 * @param  y: the y coordinate of the position
 * @param  map: the map 
 * @retval the object that corresponds to the given position
 */
MAP_OBJECT(get_object)(uint32_t x, uint32_t y, Map *map);

/**
 * @brief  opens a clue in a given position
 * @param  map: the map
 * @param  x: the x coordinate of the position
 * @param  y: the y coordinate of the position
 * @retval OK if there was a clue to open in the given position, else !OK
 */
int(map_open_clue)(Map *map, uint32_t x, uint32_t y);

/**
 * @brief  calculates the block collumn of a given x coordinate
 * @param  map: the map 
 * @param  x: the x coordinate
 * @retval the block collumn number
 */
uint16_t(get_no_block_col)(Map *map, uint32_t x);

/**
 * @brief  calculates the block row of a given y coordinate
 * @param  map: the map
 * @param  y: the y coordinate
 * @retval the block row number
 */
uint16_t(get_no_block_row)(Map *map, uint32_t y);

/**
 * @brief  calculates the block row of the door
 * @param  map: the map 
 * @retval the door row number
 */
uint16_t(get_door_row)(Map *map);

#endif
