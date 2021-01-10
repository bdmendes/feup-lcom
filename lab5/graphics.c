#include "graphics.h"

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color, bool allow_sobreposition){

  if (x >= get_mode_info()->XResolution || y >= get_mode_info()->YResolution){
    return !OK;
  }

  /* allow object sobreposition in various modes */
  if (allow_sobreposition && (color == TRANSPARENT_COLOR || color == TRANSPARENCY_COLOR_8_8_8_8) ){
    return OK;
  }

  uint8_t bits = get_mode_info()->BitsPerPixel;
  uint8_t bytes;
  uint8_t bits_per_color[4];

  switch(bits){
    case 15:
      bytes = 4;
      for (int i = 0; i < 3; ++i) bits_per_color[i] = 5;
      bits_per_color[3] = 1;
      break;
    case 24:
      bytes = 3;
      for (int i = 0; i < 3; ++i) bits_per_color[i] = 8;
      break;
    case 16:
      bytes = 3;
      bits_per_color[0] = 5;
      bits_per_color[1] = 6;
      bits_per_color[2] = 5;
      break;
    case 32:
      bytes = 4;
      for (int i = 0; i < 4; ++i) bits_per_color[i] = 8;
      break;
    case 8:
      bytes = 1;
      bits_per_color[0] = 8;
      break;
    default:
      return !OK;
  }

  char* mem = (char*) get_video_mem() + bytes * (x + y * get_mode_info()->XResolution);
  for (int i = 0; i < bytes; ++i){
    int swipe = 0;
    for (int j = 0; j < i; ++j) swipe += bits_per_color[j];
    mem[i] = (bit_range(i*bits_per_color[i],(i+1)*bits_per_color[i]) & color) >> swipe;
  }

  return OK;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color){
  for (uint16_t col = 0; col < len; ++col){
    if (vg_draw_pixel(x+col,y,color,false) != OK){
      return !OK;
    }
  }
  return OK;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
  for (uint16_t row = 0; row < height; ++row){
    if (vg_draw_hline(x,y+row,width,color) != OK){
      return !OK;
    }
  }
  return OK;
}

int (vg_draw_matrix)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step){
  uint16_t width = get_mode_info()->XResolution / no_rectangles;
  uint16_t height = get_mode_info()->YResolution / no_rectangles;

  for (int row = 0; row < no_rectangles; ++row){
    for (int col = 0; col < no_rectangles; ++col){
      uint16_t x = col*width;
      uint16_t y = row*height;

      uint32_t color = 0;
      switch (mode){
        case GRAPHICS_MODE_800x600_DIRECT: {
          uint8_t red = (get_red(first) + col * step) % (1 << get_mode_info()->RedMaskSize);
          uint8_t green = (get_green(first) + row * step) % (1 << get_mode_info()->GreenMaskSize);          
          uint8_t blue = (get_blue(first) + (col + row)*step) % (1 << get_mode_info()->BlueMaskSize);

          color = (((uint32_t) red) << 16)
          | (((uint32_t) green) << 8)
          | ((uint32_t) blue);
          break;
        }
        case GRAPHICS_MODE_1024x768_INDEXED: {
          color = (first + (row * no_rectangles + col) * step) % (1 << get_mode_info()->BitsPerPixel);
          break;
        }
        default:
          printf("mode not supported!");
          return !OK;
      }

      vg_draw_rectangle(x,y,width,height,color);
    }
  }

  return OK;
}

uint8_t get_red(uint32_t color){
  return (uint8_t) ((bit_range(16,24) & color) >> 16);
}

uint8_t get_green(uint32_t color){
  return (uint8_t) ((bit_range(8,16) & color) >> 8);
}

uint8_t get_blue(uint32_t color){
  return (uint8_t) (bit_range(0,8) & color);
}

int (vg_draw_pixmap)(uint8_t* sprite, uint16_t x, uint16_t y, xpm_image_t img){
  for (int row = 0; row < img.height ; ++row){
    for (int col = 0; col < img.width; ++col){
      vg_draw_pixel(x + col, y + row, *(sprite+col + row*img.width), true);
    }
  }
  return OK;
}

int (vg_clear_screen)(){
  uint32_t color;

  switch(get_mode_info()->BitsPerPixel){
    case 8:
      color = TRANSPARENT_COLOR;
      break;
    case 15:
      color = TRANSPARENCY_COLOR_1_5_5_5;
      break;
    case 32:
      color = TRANSPARENCY_COLOR_8_8_8_8;
      break;
    default:
      return !OK;
  }

  for (uint16_t row = 0; row < get_mode_info()->YResolution; ++row){
    for (uint16_t col = 0; col < get_mode_info()->YResolution; ++col){
      vg_draw_pixel(col,row,color,false);
    }
  }
  return OK;
}
