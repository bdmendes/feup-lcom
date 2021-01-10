#ifndef MENU_H
#define MENU_H

#include "instructions.h"
#include "settings.h"

#define MENU_TITLE_YSTART 100
#define MENU_BUTTON_YSTART 250
#define MENU_BUTTON_PADDING 150
#define MENU_BUTTON_WIDTH 400
#define MENU_BUTTON_HEIGHT 80

#define MENU_BUTTON_BACK_COLOR MILD_GRAY
#define MENU_BUTTON_HOVER_FRAME_COLOR STRONG_GRAY

/**
 * @brief The Menu holds buttons to the different game states, and a title. Stores the instructions and the settings but not the game.
 */
typedef struct Menu {
  Sprite *mouse;
  Button **buttons;
  uint8_t no_buttons;
  Sprite *back_img;
  char title[100];
  Font *font;
  Instructions *instructions;
  Settings *settings;
} Menu;

/**
 * @brief  Allocate a menu object.
 * @param  instructions: menu instructions
 * @param  settings: menu settings
 * @param  font: font used to draw the title and the buttons
 * @param  title: title to be shown above the buttons
 * @param  mouse: mouse sprite, to allow checks for button hovering
 * @param  back_img: menu background image
 * @param  no_buttons: number of buttons to be shown
 * @param  button1_text: text of the first button
 * @param  ...: remaining button texts (variable number)
 * @retval allocated menu object
 */
Menu *(create_menu)(Instructions *instructions, Settings *settings, Font *font,
                    char *title, Sprite *mouse, Sprite *back_img,
                    uint8_t no_buttons, const char *button1_text, ...);

/**
 * @brief  draws the main menu on the string (if instructions and settings are false), or the instructions/settings panel.
 * @param  menu: the menu to draw
 * @param  instructions: if set, draw instructions panel instead of main menu
 * @param  settings: if set, draw settings panel instead of main menu
 * @retval None
 */
void(draw_menu)(Menu *menu, bool instructions, bool settings);

/**
 * @brief  deallocates a menu object: all sprites and panels are destroyed, except for the font and the mouse.
 * @param  menu: the menu to destroy 
 * @retval None
 */
void(destroy_menu)(Menu *menu);

/**
 * @brief  check which button is being hovered
 * @param  menu: the menu 
 * @retval the hovered button's index; -1 if no button is being hovered
 */
int(get_hovered_button)(Menu *menu);

#endif
