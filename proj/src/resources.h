#ifndef RESOURCES_H
#define RESOURCES_H

#include "game/game.h"
#include "menu/menu.h"

/**
 * @brief Resources is a wrapper to the main objects allocated and manipulated by the app state handlers.
 */
typedef struct {
  Font *font;
  Sprite *mouse;
  Game *game;
  Menu *menu;
} Resources;

/**
 * @brief  allocates the game resources with the default values
 * @retval the created resources
 */
Resources *(create_game_resources)();

/**
 * @brief  deallocates the game resources
 * @retval None
 */
void (destroy_game_resources)(Resources *resources);

#endif
