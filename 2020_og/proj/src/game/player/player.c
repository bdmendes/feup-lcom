#include "player.h"
#include <lcom/lcf.h>

/**
 * @brief Max allowed player movement so that after moving no pixel of the player sprite is inside a ROCK block. Return 0 when it's impossible to move (hitting a ROCK)
 * @param player: the player
 * @param map: the map
 * @param dir: the direction in which the player is attempting to move
 * @retval maximum player movement (<= step)
 */
static int(get_allowed_move)(Player *player, Map *map, MOVE_DIRECTION dir);

/**
 * @brief checks if the player is within the optimal screen boundaries, which are defined in the player constructor (not in the 1/3 screen borders)
 * @param player: the player
 * @retval true if the player is within the shown area in the screen, false
 * otherwise
 */
static bool(player_is_not_in_optimal_boundaries)(Player *player);

/**
 * @brief  updates the player's absolute position in the map by making it the
 * sprite position + the current map start position
 * @param player: the player
 * @param map: the map
 * @retval None
 */
static void(update_player_absolute_pos)(Player *player, Map *map);

/**
 * @brief  decrements a player's step according to his walking direction
 * @param player: the player
 * @retval None
 */
static void(player_decrement_step)(Player *player);

/**
 * @brief checks if the player is walking vertically
 * @param player: the player
 * @retval true if the player is walking vertically, false otherwise
 */
static bool(player_is_walking_vertically)(Player *player);

/**
 * @brief assigns a direction to the player, to be used when making a guess (within player_move())
 * @param player: the player
 * @retval None
 */
static void(player_assign_direction)(Player *player);

/**
 * @brief calculates the remaining step components for the player to reach the goal_x and goal_y positions
 * @param player: the player
 * @retval None
 */
static void(player_calculate_remaining_step)(Player *player);

/**
 * @brief moves the player and adjusts the camera accordingly; if guessing, can't move in the current assigned direction, and player is not in the goal position yet, makes at maximum a new direction guess and tries to move again.
 * @param player: the player
 * @param map: the map
 * @retval None
 */
static void(player_move)(Player *player, Map *map);

/**
 * @brief guesses a new direction to move for mouse movement
 * @param player: the player
 * @retval None
 */
static void(player_make_new_move_guess)(Player *player);

Player *(create_player)(AnimSprite *asp, uint16_t speed, uint16_t steps_per_second) {
  Player *player = (Player *)malloc(sizeof(Player));
  if (player == NULL)
    return NULL;
  player->asp = asp;
  player->speed = speed;

  /* movement */
  player->force_move = false;
  player->is_moving = false;
  player->step_per_second = steps_per_second;
  player->goal_x = 0;
  player->goal_y = 0;
  player->remaining_step_x = 0;
  player->remaining_step_y = 0;
  player->is_guessing = false;

  /* should start at initial zoom buf */
  player->x_abs = asp->cur_sp->x;
  player->y_abs = asp->cur_sp->y;

  /* keep player centered around the map */
  vbe_mode_info_t* mode_info = get_mode_info();
  player->optimal_pos_boundaries[0] = mode_info->XResolution / 3;
  player->optimal_pos_boundaries[1] = 2*mode_info->XResolution / 3;
  player->optimal_pos_boundaries[2] = mode_info->YResolution / 3;
  player->optimal_pos_boundaries[3] = 2*mode_info->YResolution / 3;

  return player;
}

void(destroy_player)(Player *player) {
  destroy_asprite(player->asp);
  player->asp = NULL;
}

static bool(player_is_walking_vertically)(Player *player) {
  return player->direction == UP || player->direction == DOWN;
}

static void(player_assign_direction)(Player *player) {
  if (!player_is_walking_vertically(player) && (player->remaining_step_y)) {
    player->direction = player->remaining_step_y > 0 ? DOWN : UP;
  } else if (player_is_walking_vertically(player) &&
             (player->remaining_step_x)) {
    player->direction = player->remaining_step_x > 0 ? RIGHT : LEFT;
  }
}

void assign_move(Player *player, MOVE_DIRECTION dir) {
  player->direction = dir;
  if (!player->remaining_step_x && !player->remaining_step_y) {
    player->force_move = true;
  }
  if (!player_is_walking_vertically(player)) {
    player->remaining_step_x +=
        player->step_per_second * (dir == RIGHT ? 1 : -1);
  } else {
    player->remaining_step_y +=
        player->step_per_second * (dir == DOWN ? 1 : -1);
  }

  player->is_moving = true;
  vg_signal_update();
}

void(assign_mouse_move)(Player *player, int32_t goal_x, int32_t goal_y) {
  player->goal_x = goal_x;
  player->goal_y = goal_y;
  player->is_guessing = true;
  player_calculate_remaining_step(player);
  player_assign_direction(player);
  assign_move(player, player->direction);
}

static void(player_make_new_move_guess)(Player *player) {
  player_assign_direction(player);
  player_calculate_remaining_step(player);
}

static void(player_move)(Player *player, Map *map) {
  player->force_move = false;

  /* decrement step; if stuck, recalculate direction and step */
  player_decrement_step(player);
  player_calculate_remaining_step(player);

  if (player->is_guessing &&
      (player_is_walking_vertically(player) ? !player->remaining_step_y
                                            : !player->remaining_step_x)) {
    player_make_new_move_guess(player);
  }

  int16_t mov = get_allowed_move(player, map, player->direction);
  for (uint8_t max_tries = 2; max_tries--;) {
    if (mov) {
      if (player_is_not_in_optimal_boundaries(player) ||
          move_map(map, player->direction, mov) != OK) {
        asprite_move(
            player->asp, !player_is_walking_vertically(player) ? mov : 0,
            player_is_walking_vertically(player) ? mov : 0, true, true);
      }
      update_player_absolute_pos(player, map);
      break;

    } else if (player->is_guessing) {
      player_make_new_move_guess(player);
      mov = get_allowed_move(player, map, player->direction);
    }
  }

  if (mov) {
    animate_asprite(player->asp);
    if (player->direction == RIGHT)
      player->asp->should_mirror = false;
    else if (player->direction == LEFT)
      player->asp->should_mirror = true;
  } else
    player_stop(player);
}

void(player_move_timer_tick)(Player *player, Map *map, uint32_t timer_count) {
  if (player->is_moving)
    vg_signal_update();
  if (timer_count % player->step_per_second == 0 &&
      (player->remaining_step_x || player->remaining_step_y ||
       player->force_move)) {
    player_move(player, map);
  }
  if (!player->remaining_step_x && !player->remaining_step_y &&
      player->is_guessing) {
    player_stop(player);
  }
}

bool(player_is_above_bomb)(Player *player, Map *map) {
  for (uint32_t row = 0; row < player->asp->cur_sp->height; ++row) {
    for (uint32_t col = 0; col < player->asp->cur_sp->width; ++col) {
      if (get_object(player->x_abs + col, player->y_abs + row, map) == BOMB)
        return true;
    }
  }
  return false;
}

static void(player_calculate_remaining_step)(Player *player) {
  int32_t player_x = player->x_abs + (player->asp->cur_sp->width) / 2;
  int32_t player_y = player->y_abs + (player->asp->cur_sp->height);
  player->remaining_step_x = (player->goal_x - player_x) / player->speed;
  player->remaining_step_y = (player->goal_y - player_y) / player->speed;
}

static void(player_decrement_step)(Player *player) {
  if (!player->is_moving || player->is_guessing)
    return;
  if (player_is_walking_vertically(player)) {
    player->remaining_step_x += player->remaining_step_x > 0 ? -1 : 1;
  } else {
    player->remaining_step_y += player->remaining_step_y > 0 ? -1 : 1;
  }
}

void(player_stop)(Player *player) {
  player->remaining_step_x = 0;
  player->remaining_step_y = 0;
  player->is_moving = false;
  player->is_guessing = false;
}

static int(get_allowed_move)(Player *player, Map *map, MOVE_DIRECTION dir) {

  uint16_t max_step = player->speed < map->block_length * map->zoom_factor / 2
                          ? player->speed
                          : map->zoom_factor * map->block_length / 2;

  for (uint16_t step = max_step; step > 0; --step) {
    if (dir == RIGHT || dir == LEFT) {
      int32_t col = player->x_abs +
                    (dir == RIGHT ? player->asp->cur_sp->width + step : -step);
      if (col >= (int32_t)map->width * map->zoom_factor)
        continue;
      int32_t row = player->y_abs + player->asp->cur_sp->height;
      if (get_object(col, row, map) == ROCK)
        continue;
      return dir == RIGHT ? step : -step;
    }

    else if (dir == UP || dir == DOWN) {
      bool may_move = true;
      int32_t row = player->y_abs + player->asp->cur_sp->height +
                    (dir == DOWN ? step : -step - 1);
      if (row >= (int32_t)map->height * map->zoom_factor)
        continue;
      for (int32_t col = player->x_abs;
           col <= player->x_abs + player->asp->cur_sp->width; col++) {
        if (get_object(col, row, map) == ROCK) {
          may_move = false;
          break;
        }
      }
      if (may_move)
        return dir == DOWN ? step : -step;
    }
  }

  return 0;
}

static bool(player_is_not_in_optimal_boundaries)(Player *player) {
  return (player->direction == RIGHT || player->direction == LEFT)
             ? (player->asp->cur_sp->x < player->optimal_pos_boundaries[0] ||
                player->asp->cur_sp->x > player->optimal_pos_boundaries[1])
             : (player->asp->cur_sp->y < player->optimal_pos_boundaries[2] ||
                player->asp->cur_sp->y > player->optimal_pos_boundaries[3]);
}

static void(update_player_absolute_pos)(Player *player, Map *map) {
  player->x_abs = (int32_t)map->cur_x_start + player->asp->cur_sp->x;
  player->y_abs = (int32_t)map->cur_y_start + player->asp->cur_sp->y;
}
