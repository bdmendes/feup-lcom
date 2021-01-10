#include "resources.h"
#include <lcom/lcf.h>

/* the assets to be used in the game */
#include "../assets/cursor/cursor.xpm"
#include "../assets/instructions/bomb_instruction.xpm"
#include "../assets/instructions/clue_instruction.xpm"
#include "../assets/instructions/door_instruction.xpm"
#include "../assets/instructions/move_instruction.xpm"
#include "../assets/instructions/timer_instruction.xpm"
#include "../assets/player/player_an1.xpm"
#include "../assets/player/player_an2.xpm"
#include "../assets/player/player_an3.xpm"
#include "../assets/menu/maze.xpm"
#include "../assets/font/upscale3.75.xpm"

Resources* create_game_resources() {
  Resources* resources = (Resources*) malloc(sizeof(Resources));
  resources->font = create_font(30, letters_xpm);
  resources->mouse = create_sprite(cursor,500,500);
  if (resources->font == NULL || resources->mouse == NULL) return NULL;

  /* assemble game */
  resources->game = create_game(resources->mouse, create_player(create_asprite(0, 0, 1, 3, player_an1, player_an2, player_an3), 40, 4), create_map(), resources->font);
  if (resources->game == NULL) return NULL;

  /* assemble menus */
  Settings* settings = create_settings(create_panel(resources->mouse, resources->font, "Settings", true, false, 1, "Difficulty (zoom factor)"), resources->game->level, 3, "Easy", "Medium", "Hard");
  if (settings == NULL) return NULL;

  Instructions* instructions = create_instructions(create_panel(resources->mouse, resources->font, "Instructions", true, false, 5, "You have 5 min to escape.", "Move using keys or the mouse.", "Bombs explode each 30 s. Boom!","We have prepared some clues...","Have fun finding the door!"), 5, create_sprite(timer_instruction_xpm, 0, 0), create_sprite(move_instruction_xpm, 0, 0), create_sprite(bomb_instruction_xpm, 0, 0), create_sprite(clue_instruction_xpm, 0, 0), create_sprite(door_instruction_xpm, 0, 0));
  if (instructions == NULL) return NULL;
  
  resources->menu = create_menu(instructions, settings, resources->font, "Lerbirinto", resources->mouse, create_sprite(maze_xpm,0,0), 4, "Start Game", "Instructions", "Settings", "Exit");
  if (resources->menu == NULL) return NULL;
  
  return resources;
}

void destroy_game_resources(Resources* resources) {
  destroy_game(resources->game);
  free(resources->game);
  resources->game = NULL;
  destroy_menu(resources->menu);
  free(resources->menu);
  resources->menu = NULL;
  destroy_sprite(resources->mouse);
  free(resources->mouse);
  resources->mouse = NULL;
  destroy_font(resources->font);
  free(resources->font);
  resources->font = NULL;
}
