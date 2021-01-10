#ifndef PLAYER_H
#define PLAYER_H

#include "../../lib/sprite/asprite.h"
#include "../map/map.h"

/**
 * @brief A Player is composed of its sprite and a series of attributes related to the its movement in the map, e.g x_abs and y_abs representing the absolute position in the zoomed map, and speed the step of the sprite per movement.
 */
typedef struct Player {
  AnimSprite *asp;
  uint16_t speed;
  uint16_t optimal_pos_boundaries[4];
  int32_t x_abs;
  int32_t y_abs;
  bool force_move;
  bool is_moving;
  int32_t remaining_step_x;
  int32_t remaining_step_y;
  int32_t goal_x;
  int32_t goal_y;
  bool is_guessing;
  uint32_t step_per_second;
  MOVE_DIRECTION direction;
} Player;

/**
 * @brief allocates a player
 * @param asp: the player's sprite
 * @param speed: the player's speed
 * @param step_per_second: the player's steps per second
 * @retval the player created
 */
Player *(create_player)(AnimSprite *asp, uint16_t speed, uint16_t step_per_second);

/**
 * @brief  deallocates a player
 * @param  player: the player to destroy
 * @retval None
 */
void(destroy_player)(Player *player);

/**
 * @brief  verifies if the player is above a bomb, ie. at least one of its sprite pixels is above a block bomb
 * @param  player: the player
 * @param  map: the map
 * @retval true if the player is above a bomb, otherwise false
 */
bool(player_is_above_bomb)(Player *player, Map *map);

/**
 * @brief  assigns a move to a player in the desired direction (steps per second are added to the remaining step)
 * @param  player: the player
 * @param  dir: the direction in which the player will move
 * @retval None
 */
void (assign_move)(Player *player, MOVE_DIRECTION dir);

/**
 * @brief  stops the player's movement: remaining moves are reset and is_guessing is set to false.
 * @param  player: the player
 * @retval None
 */
void (player_stop)(Player *player);

/**
 * @brief  assigns a move made by mouse click: the directions will be guessed based on the relative difference of the x and y coordinates.
 * @param  player: the player
 * @param  goal_x: the x coordinate of the destination
 * @param  goal_y: the y coordinate of the destination
 * @retval None
 */
void (assign_mouse_move)(Player *player, int32_t goal_x, int32_t goal_y);

/**
 * @brief  moves the player in sync with the timer, according to the steps per second
 * @param  player: the player
 * @param  map: the map
 * @param  timer_count: the current timer counter value 
 * @retval None
 */
void (player_move_timer_tick)(Player *player, Map *map, uint32_t timer_count);

#endif
