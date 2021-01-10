#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "lib/panel/panel.h"

/**
 * @brief  Instructions is a panel wrapper with big padding between the text, aligned left, and a corresponding image to each text string.
 */
typedef struct Instructions {
  Panel *panel;
  Sprite **images;
  uint8_t no_images;
} Instructions;

/**
 * @brief  allocates game instructions object
 * @param  panel: the panel to show the instructions on
 * @param  no_images: the number of images
 * @param  img1: the first image
 * @param  ...: the rest of the images (variable number of arguments)
 * @retval the created game instructions
 */
Instructions *(create_instructions)(Panel *panel, uint8_t no_images,
                                    Sprite *img1, ...);

/**
 * @brief  shows the instructions on the screen
 * @param  instructions: the instructions to show
 * @retval None
 */
void(draw_instructions)(Instructions *instructions);

/**
 * @brief  deallocates a game instructions object
 * @param  instructions: the game instructions
 * @retval None
 */
void(destroy_instructions)(Instructions *instructions);

#endif
