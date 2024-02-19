#include "menu.h"
#include <lcom/lcf.h>
#include <stdarg.h>

/**
 * @brief copy the menu background to the current video buffer
 * @param menu: the menu which background shall be drawn
 */
static void (draw_menu_background)(Menu* menu);

Menu* (create_menu)(Instructions* instructions, Settings* settings, Font* font, char* title, Sprite* mouse, Sprite* back_img, uint8_t no_buttons, const char* button1_text, ...){
  Menu* menu = (Menu*) malloc(sizeof(Menu));
  if (menu == NULL) return NULL;
  menu->mouse = mouse;
  menu->font = font;
  menu->back_img = back_img;
  menu->no_buttons = no_buttons;
  menu->instructions = instructions;
  menu->settings = settings;
  strcpy(menu->title, title);
  menu->buttons = (Button**) malloc(no_buttons * sizeof(Button*));
  if (menu->buttons == NULL) return NULL;

  va_list ap;
  va_start(ap, button1_text);
  menu->buttons[0] = create_button_centered(font, button1_text, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, MENU_BUTTON_YSTART, MENU_BUTTON_BACK_COLOR, MENU_BUTTON_HOVER_FRAME_COLOR);

  for (uint8_t i = 1; i < no_buttons; ++i){
    menu->buttons[i] = create_button_centered(font, va_arg(ap, const char*), MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, MENU_BUTTON_YSTART + i*MENU_BUTTON_PADDING, MENU_BUTTON_BACK_COLOR, MENU_BUTTON_HOVER_FRAME_COLOR);
  }

  va_end(ap);
  return menu;
}

void (draw_menu)(Menu* menu, bool instructions, bool settings){
  draw_menu_background(menu);

  if (instructions){
    draw_instructions(menu->instructions);
  }
  else if (settings){
    draw_settings(menu->settings);
  }
  else {
    draw_string_centered(menu->font, menu->title, MENU_TITLE_YSTART, true);
    for (uint8_t i = 0; i < menu->no_buttons; ++i){
      draw_button(menu->buttons[i], is_inside_sprite(menu->mouse->x, menu->mouse->y,menu->buttons[i]->sp));
    }
  }
}

static void (draw_menu_background)(Menu* menu){
  memcpy(get_current_video_buf(), menu->back_img->map, get_bytes_per_pixel()*menu->back_img->width*menu->back_img->height);
}

void (destroy_menu)(Menu* menu){
  destroy_sprite(menu->back_img);
  for (uint8_t i = 0; i < menu->no_buttons; ++i) {
    destroy_button(menu->buttons[i]);
    free(menu->buttons[i]);
    menu->buttons[i] = NULL;
  }
  free(menu->buttons);
  menu->buttons = NULL;
  destroy_instructions(menu->instructions);
}

int (get_hovered_button)(Menu* menu){
  for (uint8_t i = 0; i < menu->no_buttons; ++i){
    if (is_inside_sprite(menu->mouse->x,menu->mouse->y,menu->buttons[i]->sp)) return i;
  }
  return -1;
}
