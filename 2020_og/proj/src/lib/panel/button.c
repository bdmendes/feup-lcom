#include "button.h"

Button* (create_button)(Font* font, const char* text, uint16_t width, uint16_t height, int16_t x, int16_t y, BUTTON_COLOR back_color, BUTTON_COLOR hover_frame_color){

  Button* button = (Button*) malloc(sizeof(Button));
  if (button == NULL) return NULL;
  strcpy(button->text, text);
  button->font = font;
  button->back_color = back_color;
  button->hover_frame_color = hover_frame_color;

  uint8_t bytes = get_bytes_per_pixel();
  char* main_buf = (char*) malloc(height*width*bytes);
  char* hover_buf = (char*) malloc(height*width*bytes);
  if (main_buf == NULL || hover_buf == NULL) return NULL;

  /* assemble main sprite */
  for (uint32_t i = 0; i < height*width; ++i){
    memcpy(main_buf + bytes * i, &back_color, bytes);
  }
  button->sp = create_sprite_from_bitmap(main_buf, width, height, x, y);
  if (button->sp == NULL) return NULL;

  /* assemble hover sprite */
  for (uint16_t row = 0; row < height; ++row){
    for (uint16_t col = 0; col < width; ++col){
      uint32_t color;
      if (row < BUTTON_HOVER_FRAME_WIDTH || height - row - 1 < BUTTON_HOVER_FRAME_WIDTH || col < BUTTON_HOVER_FRAME_WIDTH || width - col - 1 < BUTTON_HOVER_FRAME_WIDTH) color = hover_frame_color;
      else color = back_color;
      memcpy(hover_buf + bytes* (col + row*width), &color, bytes);      
    }
  }
  button->hover_sp = create_sprite_from_bitmap(hover_buf, width, height, x, y);
  if (button->hover_sp == NULL) return NULL;

  return button;
}

Button* (create_button_centered)(Font* font, const char* text, uint16_t width, uint16_t height, int16_t y, BUTTON_COLOR back_color, BUTTON_COLOR hover_frame_color){
  uint16_t x = get_mode_info()->XResolution/2 - width / 2;
  if (x >= get_mode_info()->XResolution) x = 0;
  return create_button(font, text, width, height, x, y, back_color, hover_frame_color);
}

Button* (create_button_letter_sized)(Font* font, char* text, int16_t x, int16_t y, BUTTON_COLOR back_color, BUTTON_COLOR hover_frame_color){
  uint16_t height = font->tile_size + 2*BUTTON_IDEAL_FRAME;
  uint16_t width = font->tile_size*strlen(text) + 2*BUTTON_IDEAL_FRAME;
  return create_button(font, text, width, height, x, y, back_color, hover_frame_color);
}

void (draw_button)(Button* button, bool hover){
  draw_sprite_no_checks(hover ? button->hover_sp : button->sp);
  draw_string(button->font, button->text, button->sp->x + (button->sp->width - strlen(button->text)*button->font->tile_size) /2, button->sp->y + (button->sp->height - button->font->tile_size) /2, false);
}

void (button_set_colors)(Button* button, BUTTON_COLOR back_color, BUTTON_COLOR hover_frame_color){
  if (button->back_color == back_color && button->hover_frame_color == hover_frame_color) return;

  uint32_t bytes = get_bytes_per_pixel();
  for (uint16_t row = 0; row < button->sp->height; ++row){
    for (uint16_t col = 0; col < button->sp->width; ++col){
      bool is_frame = row < BUTTON_HOVER_FRAME_WIDTH || button->sp->height - row - 1 < BUTTON_HOVER_FRAME_WIDTH || col < BUTTON_HOVER_FRAME_WIDTH || button->sp->width - col - 1 < BUTTON_HOVER_FRAME_WIDTH;
      if (button->back_color != back_color){
        memcpy(button->sp->map + bytes*(col+row*button->sp->width), &back_color, bytes);
        if (!is_frame){
          memcpy(button->hover_sp->map + bytes*(col+row*button->sp->width), &back_color, bytes);
        }
      }
      if (button->hover_frame_color != hover_frame_color && is_frame){
        memcpy(button->hover_sp->map + bytes*(col+row*button->sp->width), &hover_frame_color, bytes);
      }
    }
  }

  button->hover_frame_color = hover_frame_color;
  button->back_color = back_color;
}

void (destroy_button)(Button* button){
  destroy_sprite(button->sp);
  destroy_sprite(button->hover_sp);
}

void (button_set_position)(Button* button, uint16_t x, uint16_t y){
  if (x + button->sp->width > get_mode_info()->XResolution || y + button->sp->height > get_mode_info()->YResolution) return;
  button->sp->x = button->hover_sp->x = x;
  button->sp->y = button->hover_sp->y = y;
}
