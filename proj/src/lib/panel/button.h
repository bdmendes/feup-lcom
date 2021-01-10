#ifndef BUTTON_H
#define BUTTON_H

#include "../font/font.h"
#include "../sprite/sprite.h"
#include <lcom/lcf.h>

/**
 * @brief  the various possible button colors
 */
typedef enum BUTTON_COLOR {
  PURE_WHITE = 0xffffff,
  PURE_BLACK = 0x000000,
  MILD_GRAY = 0x00666666,
  MILD_GREEN = 0x00008800,
  MILD_RED = 0x8800000,
  LIGHT_GRAY = 0x00AAAAAA,
  STRONG_GRAY = 0x00444444
} BUTTON_COLOR;

#define BUTTON_HOVER_FRAME_WIDTH 5
#define BUTTON_IDEAL_FRAME 10

/**
 * @brief  A Button is composed by two sprites - one with solid color, and one with colored frame to represent the hover - and a text that will be written in its center (with the desired font).
 */
typedef struct Button {
  Sprite *sp;
  Sprite *hover_sp;
  char text[100];
  Font *font;
  BUTTON_COLOR back_color;
  BUTTON_COLOR hover_frame_color;
} Button;

/**
 * @brief  allocates a button object
 * @param  font: the desired text font
 * @param  text: the desired text
 * @param  width: the desired button width
 * @param  height: the desired button height
 * @param  x: the x coordinate of the desired position
 * @param  y: the y coordinate of the desired position
 * @param  back_color: the desired background color
 * @param  hover_frame_color: the desired color to show when hovering the button
 * @retval the created button
 */
Button *(create_button)(Font *font, const char *text, uint16_t width, uint16_t height, int16_t x, int16_t y, BUTTON_COLOR back_color, BUTTON_COLOR hover_frame_color);

/**
 * @brief  allocates a button object according to the text's size
 * @param  font: the desired font
 * @param  text: the desired text
 * @param  x: the x coordinate of the desired position
 * @param  y: the y coordinate of the desired position
 * @param  back_color: the desired background color
 * @param  hover_frame_color: the desired frame color to show when hovering the button
 * @retval the created button 
 */
Button *(create_button_letter_sized)(Font *font, char *text, int16_t x, int16_t y, BUTTON_COLOR back_color, BUTTON_COLOR hover_frame_color);

/**
 * @brief  allocates a button object that is centered in the screen
 * @param  font: the desired text font
 * @param  text: the desired text
 * @param  width: the desired button width
 * @param  height: the desired button height
 * @param  x: the x coordinate of the desired position
 * @param  back_color: the desired background color
 * @param  hover_frame_color: the desired frame color to show when hovering the button
 * @retval the created button
 */
Button *(create_button_centered)(Font *font, const char *text, uint16_t width,
                                 uint16_t height, int16_t y,
                                 BUTTON_COLOR back_color,
                                 BUTTON_COLOR hover_frame_color);

/**
 * @brief  sets a button background and frame (when hovering) colors 
 * @param  button: the button
 * @param  back_color: the value of the desired background color
 * @param  hover_frame_color: the value of the desired frame hovering color
 * @retval None
 */
void(button_set_colors)(Button *button, BUTTON_COLOR back_color,
                        BUTTON_COLOR hover_frame_color);

/**
 * @brief  draws a button to the video buffer
 * @param  button: the button 
 * @param  hover: whether or not the button is being hovered
 * @retval None
 */
void(draw_button)(Button *button, bool hover);

/**
 * @brief  deallocates a button object
 * @param  button: the button to destroy
 * @retval None
 */
void(destroy_button)(Button *button);

/**
 * @brief  sets a button's position
 * @param  button: the button
 * @param  x: the x coordinate of the desired button's position
 * @param  y: the y coordinate of the desired button's position
 * @retval None
 */
void(button_set_position)(Button *button, uint16_t x, uint16_t y);

#endif
