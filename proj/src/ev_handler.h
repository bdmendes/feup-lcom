#ifndef EV_HANDLER_H
#define EV_HANDLER_H

#include "game/game.h"
#include "menu/menu.h"

/**
 * @brief  the different possible states of the program
 */
typedef enum APP_STATE {
  MAIN_MENU,
  INSTRUCTIONS,
  SETTINGS,
  IN_GAME,
  EXIT,
  IN_GAME_CLUE,
  IN_GAME_END_MESSAGE
} APP_STATE;

/**
 * @brief  the different possible types of interrupts
 */
typedef enum INTERRUPT_TYPE { TIMER, KEYBOARD, MOUSE, RTC } INTERRUPT_TYPE;

/**
 * @brief  handles the events while on the main menu. The values passed not related to the interrupt type are not considered.
 * @param  game: the game
 * @param  menu: the menu
 * @param  interrupt: the received interrupt
 * @param  pckt: the assembled mouse packet
 * @param  kbd_code: the keyboard scancode
 * @param  state: the state
 * @retval the next state
 */
APP_STATE (handle_main_menu)(Game *game, Menu *menu, INTERRUPT_TYPE interrupt,
                           const struct packet *pckt, uint8_t kbd_code,
                           APP_STATE state);

/**
 * @brief  handles the events while in game. The values passed not related to the interrupt type are not considered.
 * @param  game: the game
 * @param  interrupt: the received interrupt
 * @param  pckt: the assembled mouse packet
 * @param  kbd_code: the keyboard scancode
 * @retval the next state
 */
APP_STATE (handle_in_game)(Game *game, INTERRUPT_TYPE interrupt,
                         const struct packet *pckt, uint8_t kbd_code);

/**
 * @brief  handles the events while on a game panel (clue or endgame message). The values passed not related to the interrupt type are not considered.
 * @param  game: the game
 * @param  interrupt: the received interrupt
 * @param  pckt: the assembled mouse packet
 * @param  kbd_code: the keyboard scancode
 * @param  state: the state
 * @retval the next state
 */
APP_STATE (handle_in_game_panel)(Game *game, INTERRUPT_TYPE interrupt,
                               const struct packet *pckt, uint8_t kbd_code,
                               APP_STATE state);

#endif
