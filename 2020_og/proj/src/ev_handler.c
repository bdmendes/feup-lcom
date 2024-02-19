#include "ev_handler.h"
#include "lib/devices/kbc/keyboard.h"
#include "lib/devices/timer/timer.h"

APP_STATE (handle_main_menu)(Game* game, Menu* menu, INTERRUPT_TYPE interrupt, const struct packet* pckt, uint8_t kbd_code, APP_STATE state){
  switch(interrupt){
    case TIMER:
      draw_menu(menu, state == INSTRUCTIONS, state == SETTINGS);
      break;
    case KEYBOARD:
      if (kbd_code == get_break_code(ESC_MAKECODE)){
        vg_signal_update();
        return state == MAIN_MENU ? EXIT : MAIN_MENU;
      }
      break;
    case MOUSE:
      if (pckt->lb && !pckt->rb){
        if (state == MAIN_MENU){
          int button_no = get_hovered_button(menu);
          if (button_no != -1) vg_signal_update();
          if (button_no == 0) return IN_GAME;
          if (button_no == 1) return INSTRUCTIONS;
          if (button_no == 2) return SETTINGS;
          if (button_no == 3) return EXIT;
        }
        else if (state == INSTRUCTIONS){
          if (is_inside_sprite(menu->mouse->x, menu->mouse->y, menu->instructions->panel->exit_button->sp)){
            vg_signal_update();
            return MAIN_MENU;
          }
        }
        else if (state == SETTINGS){
          if (is_inside_sprite(menu->mouse->x, menu->mouse->y, menu->settings->panel->exit_button->sp)){
            vg_signal_update();
            return MAIN_MENU;
          }
          for (uint8_t i = 0; i < menu->settings->no_buttons; ++i){
            if (is_inside_sprite(menu->mouse->x, menu->mouse->y, menu->settings->level_buttons[i]->sp)){
              game->level = i;
              settings_highlight_button(menu->settings, i);
              unload_game(game);
            }
          }
        }
      }
      break;
    default:
      break;
  }
  return state;
}

APP_STATE (handle_in_game)(Game* game, INTERRUPT_TYPE interrupt, const struct packet* pckt, uint8_t kbd_code){
  if (!game->is_loaded) load_game(game);
  switch(interrupt){
    case TIMER:
      player_move_timer_tick(game->player, game->map, get_timer_count());
      draw_game(game, false);
      break;


    case KEYBOARD:
      if (kbd_code == get_break_code(ESC_MAKECODE)){
        unload_game(game);
        vg_signal_update();
        return MAIN_MENU;
      }
      else if (!is_make_code(kbd_code) && kbd_code != FIRST_OF_TWO_BYTES) {
        player_stop(game->player);
      } 
      else if (kbd_code == W_MAKECODE || kbd_code == UP_ARROW_MAKECODE){
        assign_move(game->player, UP);
      }
      else if (kbd_code == S_MAKECODE || kbd_code == DOWN_ARROW_MAKECODE){
        assign_move(game->player, DOWN);
      }
      else if (kbd_code == A_MAKECODE || kbd_code == LEFT_ARROW_MAKECODE){
        assign_move(game->player, LEFT);
      }
      else if (kbd_code == D_MAKECODE || kbd_code == RIGHT_ARROW_MAKECODE){
        assign_move(game->player, RIGHT);
      }
      break;

    case MOUSE: 
      if (pckt->lb && !pckt->rb && !pckt->delta_x && !pckt->delta_y){
        if (is_inside_sprite(game->mouse->x, game->mouse->y, game->exit_button->sp)){
          unload_game(game);
          return MAIN_MENU;          
        }
        else {
          if (map_defuse_bomb(game->map, game->mouse->x + game->map->cur_x_start, game->mouse->y + game->map->cur_y_start) != OK){
            if (map_open_clue(game->map, game->mouse->x + game->map->cur_x_start, game->mouse->y + game->map->cur_y_start) == OK){
              prepare_game_panel_clue(game);
              return IN_GAME_CLUE;
            } else {
              assign_mouse_move(game->player, game->map->cur_x_start + game->mouse->x, game->map->cur_y_start + game->mouse->y);
            }
          }
        }
      }
      break;

    case RTC:
      if (!game->player->asp->is_idle) asprite_go_idle(game->player->asp);
      game_handle_countdown(game);
      FINISH_GAME_TYPE finish = get_game_finish(game);
      switch (finish){
        case END: case BOMB_EXPLODED: case TIMER_DOWN:
          prepare_game_panel_endgame(game, finish);
          return IN_GAME_END_MESSAGE;
        default:
          break;
      }
    
    default:
      break;
  }

  return IN_GAME;
}

APP_STATE (handle_in_game_panel)(Game* game, INTERRUPT_TYPE interrupt, const struct packet* pckt, uint8_t kbd_code, APP_STATE state){
  if (!game->is_loaded) load_game(game);

  switch(interrupt){
    case TIMER:
      draw_game(game, true);
      break;

    case KEYBOARD:
      if (kbd_code == get_break_code(ESC_MAKECODE)){
        vg_signal_update();
        if (state == IN_GAME_END_MESSAGE){
          unload_game(game);
          return MAIN_MENU;
        }
        else return IN_GAME;
      }
      break;

    case MOUSE: 
      if (pckt->lb && !pckt->rb && !pckt->delta_x && !pckt->delta_y){
        vg_signal_update();
        if (is_inside_sprite(game->mouse->x, game->mouse->y, game->curr_panel->exit_button->sp)){
          if (state == IN_GAME_END_MESSAGE){
            unload_game(game);
            return MAIN_MENU;
          }
          else return IN_GAME;
        }
      }
      break;
    
    default:
      break;
  }

  return state;
}

