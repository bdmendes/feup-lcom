#include "sprite.h"

Sprite* (create_sprite)(const char* pic[], int32_t x, int32_t y) {

  Sprite* sp = (Sprite*) malloc (sizeof(Sprite));
  if (sp == NULL) return NULL;

  /* read the sprite pixmap */
  xpm_image_t img;
  sp->map = (char *) xpm_load(pic, XPM_8_8_8_8, &img);
  if (sp->map == NULL) {
    free(sp);
    sp = NULL;
    return NULL;
  }

  sp->width = img.width;
  sp->height = img.height;
  sp->x = x;
  sp->y = y;
  return sp;
}

Sprite* (create_sprite_from_bitmap)(char* map, uint16_t width, uint16_t height, int32_t x, int32_t y) {
  Sprite* sp = (Sprite*) malloc(sizeof(Sprite));
  if (sp == NULL) return NULL;
  sp->map = map;
  sp->width = width;
  sp->height = height;
  sp->x = x;
  sp->y = y;
  return sp;
}

void (destroy_sprite)(Sprite* sp) {
  free(sp->map);
  sp->map = NULL;
}

int (draw_sprite)(char* buffer, Sprite* sp){
  vbe_mode_info_t* mode_info = get_mode_info();
  uint8_t bytes = get_bytes_per_pixel();
  
  for (uint16_t row = 0; row < sp->height; ++row){
    int32_t y = sp->y + row;
    if (y >= mode_info->YResolution) break;
    if (y < 0) continue;
    for (uint16_t col = 0; col < sp->width; ++col){
      int32_t x = sp->x + col;
      if (x >= mode_info->XResolution) break;
      if (x < 0) continue;
      uint32_t color = 0;
      memcpy(&color, sp->map + bytes*(col + row*sp->width), bytes);
      draw_pixel(buffer,x,y,color);
    }
  }
  return OK;
}

void (draw_sprite_no_checks)(Sprite* sp){
  vbe_mode_info_t* mode_info = get_mode_info();
  uint8_t bytes = get_bytes_per_pixel();
  char* video_buf = get_current_video_buf();
  if (sp->width == mode_info->XResolution && sp->height == mode_info->YResolution){
    memcpy(video_buf, sp->map, bytes*sp->width*sp->height);
  }
  else {
    for (uint16_t row = 0; row < sp->height; ++row){
      memcpy(video_buf + bytes*(sp->x + (sp->y+row)*mode_info->XResolution), sp->map + bytes*row*sp->width, bytes*sp->width);
    }
  }
}

int (move_sprite)(Sprite *fig, int16_t xmov, int16_t ymov, bool allow_image_cut_left_up, bool allow_image_cut_right_down) {
  if (xmov == 0 && ymov == 0) return !OK;
  int16_t new_x = fig->x + xmov, new_y = fig->y + ymov;
  vbe_mode_info_t* mode_info = get_mode_info();

  if (allow_image_cut_right_down || new_x + fig->width <= mode_info->XResolution) {
    if ((allow_image_cut_left_up || new_x >= 0) && new_x < mode_info->XResolution && new_x + fig->width > 0) fig->x = new_x;
  }

  if (allow_image_cut_right_down || new_y + fig->height <= mode_info->YResolution){
    if ((allow_image_cut_left_up || new_y >= 0) && new_y < mode_info->YResolution && new_y + fig->height > 0) fig->y = new_y;
  }

  if (fig->x == new_x || fig->y == new_y){
    vg_signal_update();
    return OK;
  }
  return !OK;
}

char* (get_mirrored_pixmap)(const char* pixmap, uint16_t width, uint16_t height){
  uint8_t bytes = get_bytes_per_pixel();
  char* mirror = (char*) malloc(height*width*bytes);
  for (uint16_t row = 0; row < height; ++row){
    for (uint16_t col = 0; col < width; ++col){
      memcpy(mirror + col*bytes + row*width*bytes, pixmap + (width-1-col)*bytes + row*width*bytes, bytes);
    }
  }
  return mirror;
}

inline bool (is_inside_sprite)(uint16_t x, uint16_t y, Sprite* sp){
  return (x >= sp->x) && (x <= sp->x + sp->width) && (y >= sp->y) && (y <= sp->y + sp->height);
}

void (draw_sprite_zoom)(char* og_buf, char* zoom_buf, Sprite* sp, uint8_t zoom_factor){

  /* draw in original buffer first */
  draw_sprite(og_buf, sp);

  /* clone to the zoomed buffer */
  uint8_t bytes = get_bytes_per_pixel();
  vbe_mode_info_t* mode_info = get_mode_info();
  uint32_t* clone_pix = (uint32_t*) malloc(bytes*zoom_factor);

  for (uint16_t pix_row = sp->y; pix_row < sp->y + sp->height; ++pix_row){
    for (uint16_t pix_col = sp->x; pix_col < sp->x + sp->width; ++pix_col){
      uint32_t* pix = (uint32_t*) (og_buf + bytes * (pix_col + pix_row*mode_info->XResolution));
      for (uint8_t clone = 0; clone < zoom_factor; ++clone) clone_pix[clone] = *pix;
      for (uint8_t y_clone = 0; y_clone < zoom_factor; ++y_clone){
        memcpy (zoom_buf + bytes * ( (pix_row*zoom_factor + y_clone)*zoom_factor*mode_info->XResolution + pix_col*zoom_factor), clone_pix, bytes*zoom_factor);
      }
    }
  }

  free(clone_pix);
  clone_pix = NULL;
}

Sprite* (create_cloned_sprite)(Sprite* sp, uint8_t zoom_factor){
  uint8_t bytes = get_bytes_per_pixel();
  uint32_t clone_pix[zoom_factor];
  char* zoom_map = (char*) malloc(zoom_factor*zoom_factor*bytes*sp->width*sp->height);
  
  for (uint16_t pix_row = 0; pix_row < sp->height; ++pix_row){
    for (uint16_t pix_col = 0; pix_col < sp->width; ++pix_col){
      uint32_t* pix = (uint32_t*) (sp->map + bytes * (pix_col + pix_row*sp->width));
      for (uint8_t clone = 0; clone < zoom_factor; ++clone){
        clone_pix[clone] = *pix;
      }
      for (uint8_t y_clone = 0; y_clone < zoom_factor; ++y_clone){
        memcpy (zoom_map + bytes * ( (pix_row*zoom_factor + y_clone)*zoom_factor*sp->width + pix_col*zoom_factor), clone_pix, bytes*zoom_factor);
      }
    }
  }

  return create_sprite_from_bitmap(zoom_map, sp->width*zoom_factor, sp->height*zoom_factor, sp->x, sp->y);
}
