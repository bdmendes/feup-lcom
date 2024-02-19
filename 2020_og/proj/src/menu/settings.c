#include "settings.h"
#include <lcom/lcf.h>
#include <stdarg.h>

Settings* (create_settings)(Panel* panel, uint8_t initial_highlighted_button, uint8_t no_buttons, const char* button1_txt, ...){
  if (panel == NULL) return NULL;
  Settings* settings = (Settings*) malloc(sizeof(Settings));
  if (settings == NULL) return NULL;
  settings->level_buttons = (Button**) malloc(no_buttons * sizeof(Button*));
  if (settings->level_buttons == NULL) return NULL;
  settings->no_buttons = no_buttons;
  settings->panel = panel;
  settings->highlighted_button = initial_highlighted_button;
  
  /* assemble level buttons */
  va_list button_list;
  va_start(button_list, button1_txt);
  uint16_t button_y_pos = panel->background->y + 2*PANEL_SIDE_SPRITE_PADDING + 3*panel->font->tile_size + PANEL_BIG_STR_PADDING;
  settings->level_buttons[0] = create_button(panel->font, button1_txt, SETTINGS_BUTTON_WIDTH, SETTINGS_BUTTON_HEIGHT, panel->background->x + PANEL_SIDE_SPRITE_PADDING, button_y_pos, settings->highlighted_button == 0 ? SETTINGS_BUTTON_BACK_ACTIVE_COLOR : SETTINGS_BUTTON_BACK_INACTIVE_COLOR, SETTINGS_BUTTON_HOVER_FRAME_COLOR);

  for (uint8_t i = 1; i < settings->no_buttons; ++i){
    settings->level_buttons[i] = create_button(panel->font, va_arg(button_list, const char*), SETTINGS_BUTTON_WIDTH, SETTINGS_BUTTON_HEIGHT, panel->background->x + (i+1)*PANEL_SIDE_SPRITE_PADDING + i*settings->level_buttons[0]->sp->width, button_y_pos, settings->highlighted_button == i ? SETTINGS_BUTTON_BACK_ACTIVE_COLOR : SETTINGS_BUTTON_BACK_INACTIVE_COLOR, SETTINGS_BUTTON_HOVER_FRAME_COLOR);

    if (settings->level_buttons[i]->sp->width != settings->level_buttons[0]->sp->width || settings->level_buttons[i]->sp->height != settings->level_buttons[i]->sp->height){ // disallow different button sizes
      settings->no_buttons = i-1;
      break;
    }
  }

  return settings;
}

void (draw_settings)(Settings* settings){
  draw_panel(settings->panel);
  for (uint8_t i = 0; i < settings->no_buttons; ++i){
    draw_button(settings->level_buttons[i], is_inside_sprite(settings->panel->mouse->x, settings->panel->mouse->y, settings->level_buttons[i]->sp));
  }
}

void (settings_highlight_button)(Settings* settings, uint8_t highlighted_button){
  uint32_t back_color = 0;
  uint8_t bytes = get_bytes_per_pixel();
  for (uint8_t i = 0; i < settings->no_buttons; ++i){
    memcpy(&back_color, settings->level_buttons[i]->sp->map, bytes);
    if (i == highlighted_button && back_color != SETTINGS_BUTTON_BACK_ACTIVE_COLOR){
      button_set_colors(settings->level_buttons[i], SETTINGS_BUTTON_BACK_ACTIVE_COLOR, SETTINGS_BUTTON_HOVER_FRAME_COLOR);
      settings->highlighted_button = i;
    }
    else if (i != highlighted_button && back_color != SETTINGS_BUTTON_BACK_INACTIVE_COLOR){
      button_set_colors(settings->level_buttons[i], SETTINGS_BUTTON_BACK_INACTIVE_COLOR, SETTINGS_BUTTON_HOVER_FRAME_COLOR);
    }
  }
}

void (destroy_settings)(Settings* settings){
  destroy_panel(settings->panel);
  for (uint8_t i = 0; i < settings->no_buttons; ++i){
    destroy_button(settings->level_buttons[i]);
    free(settings->level_buttons[i]);
    settings->level_buttons[i] = NULL;
  }
  free(settings->level_buttons);
  settings->level_buttons = NULL;
}
