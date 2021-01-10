#include "panel.h"
#include <lcom/lcf.h>
#include <stdarg.h>

Panel* (create_panel)(Sprite* mouse, Font* font, char* title, bool big_padding, bool center_text, uint8_t no_strings, const char* str1, ...){

  if (mouse == NULL || font == NULL) return NULL;
  Panel* panel = (Panel*) malloc(sizeof(Panel));
  if (panel == NULL) return NULL;
  panel->no_strings = no_strings;
  panel->string_padding = big_padding ? PANEL_BIG_STR_PADDING : PANEL_SMALL_STR_PADDING;
  panel->strings = (char**) malloc(no_strings * sizeof(char*));
  if (panel->strings == NULL) return NULL;
  panel->center_text = center_text;
  panel->font = font;
  panel->mouse = mouse;
  if (panel->font == NULL || panel->mouse == NULL) return NULL;
  strcpy(panel->title, title);

  /* assemble background sprite */
  uint8_t bytes = get_bytes_per_pixel();
  uint32_t back_color = PANEL_BACKGROUND_COLOR;

  uint16_t panel_width = get_mode_info()->XResolution;
  char* background_buf = (char*) malloc(bytes*panel_width*PANEL_HEIGHT);
  for (uint16_t row = 0; row < PANEL_HEIGHT; ++row){
    for (uint16_t col = 0; col < panel_width; ++col){
      memcpy(background_buf + bytes*(col + row * panel_width), &back_color, bytes);
    }
  }
  panel->background = create_sprite_from_bitmap(background_buf, panel_width, PANEL_HEIGHT, 0, (get_mode_info()->YResolution - PANEL_HEIGHT) / 2);
  if (panel->background == NULL) return NULL;

  /* assemble text strings */
  panel->strings[0] =  (char*) malloc(sizeof(char) * PANEL_STR_MAX_LEN);
  strcpy(panel->strings[0], str1);
  va_list str_list;
  va_start(str_list, str1);
  for (uint8_t i = 1; i < panel->no_strings; ++i){
    panel->strings[i] =  (char*) malloc(sizeof(char) * PANEL_STR_MAX_LEN);
    strcpy(panel->strings[i], va_arg(str_list, char*));
  }
  va_end(str_list);

  /* assemble exit button */
  panel->exit_button = create_button_centered(font, PANEL_EXIT_BUTTON_TEXT, 2*BUTTON_IDEAL_FRAME + strlen(PANEL_EXIT_BUTTON_TEXT)*font->tile_size, 2*BUTTON_IDEAL_FRAME + font->tile_size, (get_mode_info()->YResolution - panel->background->height)/2 + panel->background->height - 2*BUTTON_IDEAL_FRAME - font->tile_size - PANEL_SIDE_SPRITE_PADDING, PANEL_BUTTON_BACK_COLOR, PANEL_BUTTON_HOVER_FRAME_COLOR);
  if (panel->exit_button == NULL) return NULL;

  return panel;
}

void (draw_panel)(Panel* panel){
  draw_sprite_no_checks(panel->background);

  if (panel->center_text){
    draw_string_centered(panel->font, panel->title, PANEL_SIDE_SPRITE_PADDING + panel->background->y, true);
    for (uint8_t i = 0; i < panel->no_strings; ++i){
      draw_string_centered(panel->font, panel->strings[i], 2*PANEL_SIDE_SPRITE_PADDING + panel->background->y + (i+2)*panel->font->tile_size + i*(panel->string_padding), false);
    }
  }
  else {
    draw_string(panel->font, panel->title, panel->background->x + PANEL_SIDE_SPRITE_PADDING, panel->background->y + PANEL_SIDE_SPRITE_PADDING, true);
    for (uint8_t i = 0; i < panel->no_strings; ++i){
      draw_string(panel->font, panel->strings[i], panel->background->x + PANEL_SIDE_SPRITE_PADDING, 2*PANEL_SIDE_SPRITE_PADDING + panel->background->y + (i+2)*panel->font->tile_size + i*(panel->string_padding), false);
    }
  }

  draw_button(panel->exit_button, is_inside_sprite(panel->mouse->x, panel->mouse->y, panel->exit_button->sp));
}

void (set_panel_strings)(Panel* panel, const char* title, const char* str1, ...){
  strcpy(panel->title, title);
  va_list str_list;
  va_start(str_list, str1);
  strcpy(panel->strings[0], str1);
  for (uint8_t i = 1; i < panel->no_strings; ++i){
    strcpy(panel->strings[i], va_arg(str_list, char*));
  }
  va_end(str_list);
}

void (destroy_panel)(Panel* panel){
  destroy_sprite(panel->background);
  for (uint8_t i = 0; i < panel->no_strings; ++i){
    free(panel->strings[i]);
    panel->strings[i] = NULL;
  }
  free(panel->strings);
  panel->strings = NULL;
  destroy_button(panel->exit_button);
  free(panel->exit_button);
  panel->exit_button = NULL;
}
