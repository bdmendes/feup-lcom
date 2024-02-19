#ifndef GAME_H
#define GAME_H

#include "../lib/panel/panel.h"
#include "map/map.h"
#include "player/player.h"

#define GAME_DURATION_LIMIT_MINUTES 5
#define GAME_DURATION_LIMIT_SECONDS 0
#define INITIAL_PLAYER_POS_X 350
#define INITIAL_PLAYER_POS_Y 350
#define GAME_BOMB_EXPLOSION_INTERVAL_SECONDS 30
#define GAME_TIMER_SECONDS_BEFORE_BOMB_TO_TURN_RED 5
#define GAME_BUTTON_BACK_COLOR MILD_GRAY
#define GAME_BUTTON_HOVER_FRAME_COLOR PURE_WHITE
#define GAME_BUTTON_TIMER_ALERT_BACK_COLOR MILD_RED
#define GAME_NUMBER_OF_CLUES 5

/**
 * @brief the different possible ways to end the game
 */
typedef enum FINISH_GAME_TYPE {
  NOT_FINISHED,
  END,
  TIMER_DOWN,
  BOMB_EXPLODED
} FINISH_GAME_TYPE;

/**
 * @brief the game dificulty levels
 */
typedef enum DIFFICULTY_LEVEL { EASY, MEDIUM, HARD } DIFFICULTY_LEVEL;

#define GAME_INITIAL_DIFFICULTY MEDIUM

/**
 * @brief A Game holds information about the mouse, the player, the map and the font to write the buttons and timer. Clues and endgame messages are stored in a panel that is mutable during the game.
 */
typedef struct Game {
  Sprite *mouse;
  Player *player;
  Map *map;
  Font *font;
  uint8_t remaining_minutes;
  uint8_t remaining_seconds;
  char countdown_text[6];
  bool is_loaded;
  Button *exit_button;
  Button *countdown_button;
  Panel *curr_panel;
  bool panel_is_clue;
  DIFFICULTY_LEVEL level;
  bool clue_found[GAME_NUMBER_OF_CLUES];
} Game;

/**
 * @brief  allocates a game
 * @param  mouse: the cursor sprite
 * @param  player: the player
 * @param  map: the map
 * @param  font: the font
 * @retval the created game
 */
Game *(create_game)(Sprite *mouse, Player *player, Map *map, Font *font);

/**
 * @brief  sets a new game dificulty level: the game is unloaded and needs to be loaded with the new map zoom buffer in the next in_game handler.
 * @param  game: the game
 * @param  level: the desired dificulty level
 * @retval None
 */
void(set_game_dificulty_level)(Game *game, DIFFICULTY_LEVEL level);

/**
 * @brief  assembles the map zoom buffer according to the current level zoom factor; resets the player to the initial position
 * @param  game: game to be loaded
 * @retval None
 */
void(load_game)(Game *game);

/**
 * @brief  decrements a second to the in_game countdown timer. If a bomb is about to explode in a few seconds, the timer background turns red; else displays the default color.
 * @param  game: the game
 * @retval None
 */
void(game_handle_countdown)(Game *game);

/**
 * @brief  draws the various game elements on the screen: the map, the player, the countdown timer and the resign button
 * @param  game: game to be drawn
 * @param  show_panel: whether to show the panel or not
 * @retval None
 */
void(draw_game)(Game *game, bool show_panel);

/**
 * @brief  checks in which way the game ended: bomb explosion (game countdown timer defined), timer run out, door found (player top left pixel is within a door), or neither (game is in progress)
 * @param  game: the game
 * @retval the finish type
 */
FINISH_GAME_TYPE(get_game_finish)(Game *game);

/**
 * @brief  signal that the game shall be loaded again in the next in_game handler call, e.g when the door has been found and the game needs to be reset, or when the difficulty level has changed and a new map zoom buffer needs to be assembled.
 * @param  game: the game
 * @retval None
 * 
 */
void(unload_game)(Game *game);

/**
 * @brief deallocats the game map buffers and player. The mouse sprite and the font are not destroyed.
 * @param  game: the game to destroy
 * @retval None
 */
void(destroy_game)(Game *game);

/**
 * @brief  makes the current game panel show the next clue that has not been shown yet.
 * @param  game: the game
 * @retval None
 */
void(prepare_game_panel_clue)(Game *game);

/**
 * @brief  makes the current game panel show a endgame message according to the finish type of the game.
 * @param  game: the game
 * @param  finish: the type of finish
 * @retval None
 */
void(prepare_game_panel_endgame)(Game *game, FINISH_GAME_TYPE finish);

#endif
