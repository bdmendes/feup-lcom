#ifndef SETTINGS_H
#define SETTINGS_H

#include "lib/panel/panel.h"

#define SETTINGS_BUTTON_WIDTH 300
#define SETTINGS_BUTTON_HEIGHT 100
#define SETTINGS_BUTTON_BACK_INACTIVE_COLOR LIGHT_GRAY
#define SETTINGS_BUTTON_BACK_ACTIVE_COLOR STRONG_GRAY
#define SETTINGS_BUTTON_HOVER_FRAME_COLOR STRONG_GRAY

/**
 * @brief Settings is a panel wrapper with aditional buttons (options) at the bottom to the first panel string.
 */
typedef struct Settings {
  Panel *panel;
  uint8_t highlighted_button;
  Button **level_buttons;
  uint8_t no_buttons;
} Settings;

/**
 * @brief  allocates a settings object
 * @param  panel: the panel to use
 * @param  initial_highlighted_button: the index of the button that is highlighted by default
 * @param  no_buttons: the number of buttons in the menu
 * @param  button1_txt: the text for the first button
 * @param  ...: remaining button texts (variable number of arguments)
 * @retval the created settings
 */
Settings *(create_settings)(Panel *panel, uint8_t initial_highlighted_button,
                            uint8_t no_buttons, const char *button1_txt, ...);

/**
 * @brief  draws the settings to the screen
 * @param  settings: the settings
 * @retval None
 */
void(draw_settings)(Settings *settings);

/**
 * @brief  highlights a button from the given settings
 * @param  settings: the settings
 * @param  highlighted_button: the index of the button to highlight
 * @retval None
 */
void(settings_highlight_button)(Settings *settings, uint8_t highlighted_button);

/**
 * @brief  deallocates a settings object
 * @param  settings: the settings to destroy
 * @retval None
 */
void(destroy_settings)(Settings *settings);

#endif
