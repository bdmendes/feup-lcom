#include "game.h"

/**
 * @brief  updates the timer button
 * @param  game: the game 
 * @retval None
 */
static void (game_update_timer_button_text)(Game* game);

/**
 * @brief  resets the clues
 * @param  game: the game
 * @retval None
 */
static void (reset_found_clues)(Game* game);

/**
 * @brief  gets the next clue that was not seen yet
 * @param  game: the game
 * @retval None
 */
static uint8_t (get_next_not_found_clue)(Game* game);

Game* (create_game)(Sprite* mouse, Player* player, Map* map, Font* font){
  if (mouse == NULL || player == NULL || map == NULL || font == NULL) return NULL;
  Game* game = (Game*) malloc(sizeof(Game));
  if (game == NULL) return NULL;
  game->player = player;
  game->map = map;
  game->font = font;
  game->mouse = mouse;
  game->is_loaded = false;
  game->remaining_minutes = GAME_DURATION_LIMIT_MINUTES;
  game->remaining_seconds = GAME_DURATION_LIMIT_SECONDS;
  game->level = GAME_INITIAL_DIFFICULTY;
  reset_found_clues(game);
  sprintf(game->countdown_text, "%02d:%02d", GAME_DURATION_LIMIT_MINUTES, GAME_DURATION_LIMIT_SECONDS);

  /* load game exit button */
  game->exit_button = create_button_letter_sized(font, "Resign", 0, BUTTON_IDEAL_FRAME, GAME_BUTTON_BACK_COLOR, GAME_BUTTON_HOVER_FRAME_COLOR);
  if (game->exit_button == NULL) return NULL;
  game->exit_button->sp->x = game->exit_button->hover_sp->x = get_mode_info()->XResolution-1 - game->exit_button->sp->width - BUTTON_IDEAL_FRAME;

  /* load game countdown timer button (not clickable) */
  game->countdown_button = create_button_letter_sized(font, game->countdown_text, BUTTON_IDEAL_FRAME, BUTTON_IDEAL_FRAME, GAME_BUTTON_BACK_COLOR, GAME_BUTTON_HOVER_FRAME_COLOR);
  if (game->countdown_button == NULL) return NULL;

  /* load modifiable panel (clues and endgame messages) */
  game->curr_panel = create_panel(game->mouse, game->font, "Clue", false, true, 3, "","","");
  if (game->curr_panel == NULL) return NULL;

  return game;
}

void (load_game)(Game* game){
  game->player->asp->cur_sp->x = game->player->x_abs = INITIAL_PLAYER_POS_X;
  game->player->asp->cur_sp->y = game->player->y_abs = INITIAL_PLAYER_POS_Y;
  player_stop(game->player);
  game->map->cur_x_start = game->map->cur_y_start = 0;
  game->remaining_minutes = GAME_DURATION_LIMIT_MINUTES;
  game->remaining_seconds = GAME_DURATION_LIMIT_SECONDS;
  reset_found_clues(game);

  uint8_t zoom_factor = EASY_ZOOM_FACTOR;
  switch (game->level){
    case MEDIUM:
      zoom_factor = MEDIUM_ZOOM_FACTOR;
      break;
    case HARD:
      zoom_factor = HARD_ZOOM_FACTOR;
      break;
    default:
      break;
  }

  button_set_colors(game->countdown_button, GAME_BUTTON_BACK_COLOR, GAME_BUTTON_HOVER_FRAME_COLOR);
  load_map(game->map, zoom_factor);
  game_update_timer_button_text(game);
  game->is_loaded = true;
}

void (game_handle_countdown)(Game* game){

  /* handle timer countdown */
  if (game->remaining_seconds != 0){
    game->remaining_seconds--;
  }
  else if (game->remaining_minutes != 0){
    game->remaining_minutes--;
    game->remaining_seconds = 59;
  }

  game_update_timer_button_text(game);

  /* turn timer red if bomb explosion soon */
  if (game->remaining_minutes != GAME_DURATION_LIMIT_MINUTES && (game->remaining_minutes*60 + game->remaining_seconds) % GAME_BOMB_EXPLOSION_INTERVAL_SECONDS <= GAME_TIMER_SECONDS_BEFORE_BOMB_TO_TURN_RED ){
    if (game->countdown_button->back_color != GAME_BUTTON_TIMER_ALERT_BACK_COLOR) button_set_colors(game->countdown_button, GAME_BUTTON_TIMER_ALERT_BACK_COLOR, GAME_BUTTON_HOVER_FRAME_COLOR);
  }
  else {
    if (game->countdown_button->back_color != GAME_BUTTON_BACK_COLOR) button_set_colors(game->countdown_button, GAME_BUTTON_BACK_COLOR, GAME_BUTTON_HOVER_FRAME_COLOR);
    if ((game->remaining_minutes*60 + game->remaining_seconds) % GAME_BOMB_EXPLOSION_INTERVAL_SECONDS == GAME_BOMB_EXPLOSION_INTERVAL_SECONDS - 1) reactivate_all_bombs(game->map);
  }

  vg_signal_update();
}

void (set_game_dificulty_level)(Game* game, DIFFICULTY_LEVEL level){
  game->level = level;
  unload_game(game);
}

FINISH_GAME_TYPE (get_game_finish)(Game* game){
  if (get_object(game->player->x_abs, game->player->y_abs + game->player->asp->cur_sp->height, game->map) == DOOR){
    return END;
  }
  if (game->remaining_minutes == 0 && game->remaining_seconds == 0){
    return TIMER_DOWN;
  }
  if (game->remaining_minutes != GAME_DURATION_LIMIT_MINUTES && (game->remaining_minutes*60 + game->remaining_seconds) % GAME_BOMB_EXPLOSION_INTERVAL_SECONDS == 0){
    if (player_is_above_bomb(game->player, game->map)) return BOMB_EXPLODED;
  }
  return NOT_FINISHED;
}

void (draw_game)(Game* game, bool show_panel){
  draw_map(game->map);
  if (!show_panel){
    draw_asprite(get_current_video_buf(), game->player->asp);
    draw_button(game->exit_button, is_inside_sprite(game->mouse->x, game->mouse->y, game->exit_button->sp));
    draw_button(game->countdown_button, false);
  }
  else {
    draw_panel(game->curr_panel);
  }
}

void (prepare_game_panel_clue)(Game* game){
  uint8_t no_clue = get_next_not_found_clue(game);
  switch (no_clue){
    case 0:
      set_panel_strings(game->curr_panel, "A bird told me...", "The door is on the right bottom.", "Thread carefully, little wizard.", "");
      break;
    case 1:{
      char aux_str1[50], aux_str2[50];
      sprintf(aux_str1, "Your current block is (%d, %d).", get_no_block_col(game->map, game->player->x_abs) + 1, get_no_block_row(game->map, game->player->y_abs) + 1);
      sprintf(aux_str2, "The map dimensions are %d x %d blocks.", game->map->no_blocks_col, game->map->no_blocks_row);
      set_panel_strings(game->curr_panel, "Some geometry...", aux_str1, aux_str2, "");
      break;
    }
    case 2:
      set_panel_strings(game->curr_panel, "Is this too easy?", "Try a different level next time.", "Oh, you were expecting a clue, right?", "Better luck next time.");
      break;
    case 3:{
      bool go_down = get_door_row(game->map) >= get_no_block_row(game->map, game->map->cur_y_start + game->player->asp->cur_sp->y);
      const char str1[] = "To find the door, you shall go down.";
      const char str2[] = "To find the door, you shall go up.";
      set_panel_strings(game->curr_panel, "Now what?", go_down ? str1 : str2, "", "");
      break;
    }
    case 4:
      set_panel_strings(game->curr_panel, "Trivia", "This was developed by Bruno Mendes", "and Rita Mendes for LCOM 2020/2021.", "Not that you were that interested.");
      break;
  }
  game->clue_found[no_clue] = true;
  vg_signal_update();
}

void (prepare_game_panel_endgame)(Game* game, FINISH_GAME_TYPE finish){
  switch (finish){
    case END:
      set_panel_strings(game->curr_panel, "Congratulations!", "You found the door to exit the maze!", "What a remarkable journey.", "");
      break;
    case TIMER_DOWN:
      set_panel_strings(game->curr_panel, "Too slow!", "You could not exit the maze in time!", "A bit of a shame, if you ask me...","");
      break;
    case BOMB_EXPLODED:
      set_panel_strings(game->curr_panel, "Boom!", "A bomb exploded right in your head!", "Next time, defuse the bombs.","Really.");
      break;
    default:
      return;
  }
  vg_signal_update();
}

void (unload_game)(Game* game){
  game->is_loaded = false;
  vg_signal_update();
}

void (destroy_game)(Game* game){
  destroy_player(game->player);
  destroy_map(game->map);
  destroy_button(game->exit_button);
  destroy_button(game->countdown_button);
  destroy_panel(game->curr_panel);
}

static inline void (game_update_timer_button_text)(Game* game){
  sprintf(game->countdown_text, "%02d:%02d", game->remaining_minutes, game->remaining_seconds);
  strcpy(game->countdown_button->text, game->countdown_text);
}

static void (reset_found_clues)(Game* game){
  for (uint8_t i = 0; i < GAME_NUMBER_OF_CLUES; ++i){
    game->clue_found[i] = false;
  }
}

static uint8_t (get_next_not_found_clue)(Game* game){
  for (uint8_t i = 0; i < GAME_NUMBER_OF_CLUES; ++i){
    if (!game->clue_found[i]) return i;
  }
  return 0;
}
