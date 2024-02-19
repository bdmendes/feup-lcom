#include "instructions.h"
#include <lcom/lcf.h>
#include <stdarg.h>

Instructions* (create_instructions)(Panel* panel, uint8_t no_images, Sprite* img1, ...){
  if (panel == NULL) return NULL;
  Instructions* instructions = (Instructions*) malloc(sizeof(Instructions));
  if (instructions == NULL) return NULL;
  instructions->panel = panel;
  instructions->no_images = no_images;
  instructions->images = (Sprite**) malloc(sizeof(Sprite*) * no_images);
  if (instructions->images == NULL) return NULL;

  /* map sprites */
  va_list img_list;
  va_start(img_list, img1);
  instructions->images[0] = img1;
  if (instructions->images[0] == NULL) return NULL;

  for (uint8_t i = 1; i < instructions->no_images; ++i){
    instructions->images[i] = va_arg(img_list, Sprite*);
    if (instructions->images[i] == NULL) return NULL;
    if (instructions->images[i]->height != instructions->images[0]->height || instructions->images[i]->width != instructions->images[0]->width){ // disallow images with different sizes
      instructions->no_images = i - 1;
      break;
    }
  }

  /* assemble sprite positions */
  for (uint8_t i = 0; i < instructions->panel->no_strings && i < instructions->no_images; ++i){
    instructions->images[i]->x = instructions->panel->background->x + instructions->panel->background->width - instructions->images[i]->width - PANEL_SIDE_SPRITE_PADDING;
    instructions->images[i]->y = panel->background->y + 2*PANEL_SIDE_SPRITE_PADDING + 2*panel->font->tile_size + i*(instructions->panel->font->tile_size + panel->string_padding) - instructions->panel->font->tile_size/2;
  }

  return instructions;
}

void (draw_instructions)(Instructions* instructions){
  draw_panel(instructions->panel);
  for (uint8_t i = 0; i < instructions->panel->no_strings && i < instructions->no_images; ++i){
    draw_sprite(get_current_video_buf(), instructions->images[i]);
  }
}

void (destroy_instructions)(Instructions* instructions){
  for (uint8_t i = 0; i < instructions->no_images; ++i){
    destroy_sprite(instructions->images[i]);
    free(instructions->images[i]);
    instructions->images[i] = NULL;
  }
  free(instructions->images);
  instructions->images = NULL;
  destroy_panel(instructions->panel);
}
