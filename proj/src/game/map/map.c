#include "map.h"
#include "../../lib/devices/graphics/vbe.h"
#include <lcom/lcf.h>

/* environment assets */
#include "../assets/environment/grass1.xpm"
#include "../assets/environment/grass_bomb.xpm"
#include "../assets/environment/grass_bomb_defused.xpm"
#include "../assets/environment/grass_clue.xpm"
#include "../assets/environment/grass_door.xpm"
#include "../assets/environment/stone1.xpm"

/**
 * @brief Draws a map block in a given position
 * @param map: the map 
 * @param object_no: the object to draw
 * @param xpos: the x coordinate of the position
 * @param ypos: the y coordinate of the position
 * @retval None
 */
static void (map_draw_block)(Map* map, MAP_OBJECT object_no, uint32_t xpos, uint32_t ypos);

/**
 * @brief Puts a given object in a given position on the map
 * @param map: the map
 * @param x: the x coordinate of the position
 * @param y: the y coordinate of the position
 * @retval OK if success, else !OK
 */
static int (map_set_object)(Map* map, uint32_t x, uint32_t y, MAP_OBJECT new_obj);

/**
 * @brief Replace a map object with another map object
 * @param map: the map
 * @param x: the x coordinate of the position
 * @param y: the y coordinate of the position
 * @param removed_object: the object to remove
 * @param added_object: the object to add
 * @retval None
 */
static int (map_replace)(Map* map, uint32_t x, uint32_t y, MAP_OBJECT removed_object, MAP_OBJECT added_object);

static MAP_OBJECT map_initial_objects[32*24] = {
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
0,2,1,0,2,0,0,0,2,1,0,0,1,0,2,0,0,0,1,0,0,0,0,0,0,2,0,2,2,0,0,1,
1,0,1,1,1,1,2,1,0,1,0,2,1,0,1,1,1,0,1,1,1,1,0,1,1,0,1,1,0,1,2,1,
1,2,0,0,0,1,0,1,1,1,1,0,1,0,3,0,1,2,1,0,0,1,2,0,1,2,1,3,2,1,1,1,
1,0,1,1,2,1,0,1,0,1,0,2,1,1,1,2,1,0,0,2,0,1,0,0,1,0,1,2,0,0,0,1,
1,1,1,0,0,1,0,0,2,0,0,0,2,1,2,0,0,2,0,0,1,1,0,0,2,0,1,0,2,1,0,1,
1,0,1,0,0,1,1,1,0,1,1,1,1,1,0,1,1,1,1,2,0,1,2,1,0,2,1,0,2,1,0,1,
1,2,2,0,2,2,0,1,0,1,0,2,0,1,0,2,0,2,1,0,0,2,0,1,1,0,1,1,0,1,2,1,
1,1,1,2,1,0,0,1,2,2,2,2,1,1,1,1,1,0,1,2,1,1,2,2,1,2,0,1,0,1,0,1,
1,0,1,0,1,1,1,1,1,1,1,0,2,0,0,2,1,0,0,0,1,0,0,0,1,0,2,0,0,2,0,1,
1,0,0,2,1,0,2,0,0,2,1,0,1,0,1,0,1,0,1,2,1,2,2,1,1,1,1,2,1,1,2,1,
1,2,1,0,1,0,0,1,1,2,1,2,1,0,1,0,1,1,1,0,1,0,0,1,0,2,0,0,2,1,0,1,
1,2,1,0,0,2,2,0,1,0,0,0,1,2,1,2,0,3,0,2,1,0,1,1,1,0,1,1,1,1,1,1,
1,2,1,1,1,1,1,0,1,2,1,0,0,0,1,0,1,1,1,1,1,2,0,1,2,2,0,0,0,1,0,1,
1,0,0,2,2,0,0,2,0,0,1,0,2,2,1,2,0,0,1,0,0,0,0,1,0,0,1,1,1,1,2,1,
1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,2,0,1,1,1,0,2,0,0,2,0,0,1,
1,2,0,0,2,1,0,2,1,3,0,0,2,0,1,0,1,2,1,2,0,1,0,0,0,1,1,1,2,1,1,1,
1,0,1,0,0,1,1,0,1,0,1,0,0,0,1,0,1,0,0,0,1,1,2,1,2,1,0,1,2,2,2,4,
1,2,1,0,0,0,1,0,1,0,1,1,1,2,1,0,1,0,1,2,0,2,0,1,0,2,0,1,2,2,2,1,
1,2,1,1,1,0,1,2,0,0,2,0,2,0,1,1,1,2,1,2,1,1,1,1,1,1,2,1,0,1,1,1,
1,0,2,0,2,2,1,0,1,1,1,1,0,2,0,0,1,0,1,0,1,0,0,0,0,2,0,1,2,0,0,1,
1,1,1,0,1,1,1,2,1,0,0,1,2,1,1,0,1,2,1,2,1,0,0,1,0,1,1,1,0,1,2,1,
1,0,0,0,0,2,0,2,1,0,0,2,0,0,0,0,1,0,0,3,0,0,0,1,0,0,1,0,0,1,0,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

Map* (create_map)() {

  Map *map = (Map *) malloc(sizeof(Map));
  if (map == NULL) return NULL;
  map->width = get_mode_info()->XResolution;
  map->height = get_mode_info()->YResolution;

  /* allocate main buf */
  uint8_t bytes = get_bytes_per_pixel();
  map->buf = malloc(sizeof(char) * map->width * map->height * bytes);
  if (map->buf == NULL) return NULL;

  /* assemble block sprites */
  map->no_block_types = 6;
  map->sprites = (Sprite**) malloc(sizeof(Sprite*) * map->no_block_types);
  if (map->sprites == NULL) return NULL;
  map->sprites[0] = create_sprite(grass1, 0, 0);
  map->sprites[1] = create_sprite(stone1, 0, 0);
  map->sprites[2] = create_sprite(grass_bomb, 0, 0);
  map->sprites[3] = create_sprite(grass_clue, 0, 0);
  map->sprites[4] = create_sprite(grass_door, 0, 0);
  map->sprites[5] = create_sprite(grass_bomb_defused, 0, 0);

  map->block_length = map->sprites[0]->width;
  map->no_blocks_row = map->width / map->block_length;
  map->no_blocks_col = map->height / map->block_length;
  map->objects = (MAP_OBJECT*) malloc(map->no_blocks_col*map->no_blocks_row*sizeof(MAP_OBJECT));
  if (map->objects == NULL) return NULL;

  /* placehold values until it's loaded */
  map->zoom_factor = 0;
  map->zoom_buf = NULL;

  return map;
}

void (load_map)(Map* map, uint8_t zoom_factor){
  if (map->zoom_buf == NULL || zoom_factor != map->zoom_factor){
    free(map->zoom_buf);
    map->zoom_buf = NULL;
    map->zoom_buf = malloc(sizeof(char) * map->width * map->height * get_bytes_per_pixel() * zoom_factor * zoom_factor);
  }
  map->cur_x_start = 0;
  map->cur_y_start = 0;
  map->zoom_factor = zoom_factor;

  memcpy(map->objects, map_initial_objects, map->no_blocks_col*map->no_blocks_row*sizeof(MAP_OBJECT));
  
  /* draw sprites in both buffers */
  for (uint16_t row = 0; row < map->no_blocks_col; ++row){
    for (uint16_t col = 0; col < map->no_blocks_row; ++col){
      map_draw_block(map, map->objects[row*map->no_blocks_row + col], col, row);
    }
  }
}

void (destroy_map)(Map *map) {
  free(map->buf);
  map->buf = NULL;
  free(map->zoom_buf);
  map->zoom_buf = NULL;
  for (uint8_t i = 0; i < map->no_block_types; ++i){
    destroy_sprite(map->sprites[i]);
    free(map->sprites[i]);
    map->sprites[i] = NULL;
  }
  free(map->sprites);
  map->sprites = NULL;
  free(map->objects);
  map->objects = NULL;
}

int (move_map)(Map* map, MOVE_DIRECTION dir, int16_t step){
  switch (dir){
    case RIGHT:
      if (step < 0) return !OK;
      do if(map->cur_x_start + map->width + step <= map->width*map->zoom_factor) break; while (step--);
      map->cur_x_start += step;
      break;
    case LEFT:
      if (step > 0) return !OK;
      do if ((int16_t) map->cur_x_start + step >= 0) break; while (step++);
      map->cur_x_start += step;
      break;
    case UP:
      if (step > 0) return !OK;
      do if ((int16_t) map->cur_y_start + step >= 0) break; while (step++);
      map->cur_y_start += step;
      break;
    case DOWN:
      if (step < 0) return !OK;
      do if (map->cur_y_start + map->height + step <= map->height*map->zoom_factor) break; while (step--);
      map->cur_y_start += step;
      break;
  }

  return step != 0 ? OK : !OK;
}

void (draw_map)(Map* map) {
  if (map->zoom_buf == NULL) return;
  uint8_t bytes = get_bytes_per_pixel();
  for (uint32_t line = 0; line < map->height; ++line){
    memcpy((void*)(get_current_video_buf() + bytes*line*map->width), map->zoom_buf + bytes*(map->cur_x_start + (map->cur_y_start+line)* map->width * map->zoom_factor), bytes*map->width);
  }
}

int (map_defuse_bomb)(Map* map, uint32_t x, uint32_t y){
  return map_replace(map, x, y, BOMB, DEFUSED_BOMB);
}

int (map_open_clue)(Map* map, uint32_t x, uint32_t y){
  return map_replace(map, x, y, CLUE, FREEWAY);
}

void (reactivate_all_bombs)(Map* map){
  for (uint16_t row = 0; row < map->no_blocks_col; ++row){
    for (uint16_t col = 0; col < map->no_blocks_row; ++col){
      if (map->objects[col + row*map->no_blocks_row] == DEFUSED_BOMB){
        map->objects[col + row*map->no_blocks_row] = BOMB;
        map_draw_block(map, BOMB, col, row);
      }
    }
  }
  vg_signal_update();
}

MAP_OBJECT (get_object)(uint32_t x, uint32_t y, Map* map){
  if (y >= map->height*map->zoom_factor || x >= map->width*map->zoom_factor){
    return !FREEWAY;
  }

  x /= (map->zoom_factor*map->block_length);
  y /= (map->zoom_factor*map->block_length);

  return map->objects[x + y*map->no_blocks_row];
}

static int (map_set_object)(Map* map, uint32_t x, uint32_t y, MAP_OBJECT new_obj){
  if (y >= map->height*map->zoom_factor || x >= map->width*map->zoom_factor){
    return !OK;
  }

  x /= (map->zoom_factor*map->block_length);
  y /= (map->zoom_factor*map->block_length);

  map->objects[x + y*map->no_blocks_row] = new_obj;
  return OK;
}

static void (map_draw_block)(Map* map, MAP_OBJECT object_no, uint32_t xpos, uint32_t ypos){
  Sprite* curr_sp = map->sprites[object_no];
  curr_sp->x = xpos * map->block_length;
  curr_sp->y = ypos * map->block_length;
  draw_sprite_zoom(map->buf, map->zoom_buf, curr_sp, map->zoom_factor);
}

uint16_t (get_no_block_col)(Map* map, uint32_t x){
  x /= (map->zoom_factor*map->block_length);
  return x;
}

uint16_t (get_no_block_row)(Map* map, uint32_t y){
  y /= (map->zoom_factor*map->block_length);
  return y;
}

uint16_t (get_door_row)(Map* map){
  for (uint16_t row = 0; row < map->no_blocks_col; ++row){
    for (uint16_t col = 0; col < map->no_blocks_row; ++col){
      if (map->objects[col + row*map->no_blocks_row] == DOOR) return row;
    }
  }
  return 0;
}

static int (map_replace)(Map* map, uint32_t x, uint32_t y, MAP_OBJECT removed_object, MAP_OBJECT added_object){
  MAP_OBJECT obj = get_object(x,y,map);
  if (obj != removed_object) return !OK;
  if (map_set_object(map,x,y,added_object) != OK) return !OK;
  map_draw_block(map, added_object, x/map->zoom_factor/map->block_length, y/map->zoom_factor/map->block_length);
  vg_signal_update();
  return OK;    
}
