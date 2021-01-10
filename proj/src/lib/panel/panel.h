#ifndef PANEL_H
#define PANEL_H

#include "button.h"

#define PANEL_HEIGHT 600
#define PANEL_SIDE_SPRITE_PADDING 50
#define PANEL_EXIT_BUTTON_TEXT "Close"
#define PANEL_SMALL_STR_PADDING 20
#define PANEL_BIG_STR_PADDING 30
#define PANEL_STR_MAX_LEN 50
#define PANEL_BACKGROUND_COLOR MILD_GRAY
#define PANEL_BUTTON_BACK_COLOR LIGHT_GRAY
#define PANEL_BUTTON_HOVER_FRAME_COLOR PURE_WHITE

/**
 * @brief A Panel is composed of a title, a solid background, text strings and a close button.
 */
typedef struct Panel {
  Button *exit_button;
  uint8_t no_strings;
  uint16_t string_padding;
  char **strings;
  char title[PANEL_STR_MAX_LEN];
  bool center_text;
  Sprite *background;
  Font *font;
  Sprite *mouse;
} Panel;

/**
 * @brief  allocates a panel object
 * @param  mouse: the used mouse's sprite
 * @param  font: the desired font
 * @param  title: the desired text
 * @param  big_padding: whether to use PANEL_BIG_STR_PADDING or
 * PANEL_SMALL_STR_PADDING for the padding between strings
 * @param  center_text: whether the text should be centered or not
 * @param  no_strings: number of strings to show
 * @param  str1: the first string to show
 * @param  ...: the rest of the strings (variable number)
 * @retval the created pannel
 */
Panel *(create_panel)(Sprite *mouse, Font *font, char *title, bool big_padding,
                      bool center_text, uint8_t no_strings, const char *str1,
                      ...);

/**
 * @brief  draws a panel to the video buffer
 * @param  panel: the panel to show
 * @retval None
 */
void(draw_panel)(Panel *panel);

/**
 * @brief  sets the panel's strings
 * @param  panel: the panel
 * @param  title: the panel's title string
 * @param  str1: the first string
 * @param  ...: the rest of the strings (variable number)
 * @retval None
 */
void(set_panel_strings)(Panel *panel, const char *title, const char *str1, ...);

/**
 * @brief  deallocates a panel object: the button and background sprites. The mouse and font are not destroyed.
 * @param  panel: the panel to destroy
 * @retval None
 */
void(destroy_panel)(Panel *panel);

#endif
