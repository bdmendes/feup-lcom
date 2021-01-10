#include "graphics.h"

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {

  vbe_mode_info_t* mode_info = vbe_get_vbe_m_info();
  uint8_t* video_mem = (uint8_t*) vbe_get_video_mem();

  if (x >= mode_info->XResolution || y >= mode_info->YResolution){
    return !OK;
  }

  uint8_t bytes_per_pixel = mode_info->BytesPerScanLine/mode_info->XResolution;
  uint16_t x_resolution = mode_info->XResolution;

  color &= bit_range(0, mode_info->BitsPerPixel);

  memcpy((char *)video_mem + bytes_per_pixel * x_resolution * y + bytes_per_pixel * x, &color, bytes_per_pixel);

  return OK;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  for (int i = y; i < y + height; i ++) {
    vg_draw_hline(x, i, width, color);
  }
  return OK;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  for (int i = x; i < x + len; i ++) {
    vg_draw_pixel(i, y, color);
  }
  return OK;
}

int (vg_draw_matrix)(uint8_t no_rectangles, uint32_t first, uint8_t step) {

  vbe_mode_info_t* vbe_m_info = vbe_get_vbe_m_info();
  uint16_t width = vbe_m_info->XResolution / no_rectangles;
  uint16_t height = vbe_m_info->YResolution / no_rectangles;
  uint8_t bytes_per_pixel = vbe_m_info->BytesPerScanLine/vbe_m_info->XResolution;

  for (uint8_t row = 0; row < no_rectangles; row ++) {
    for (uint8_t col = 0; col < no_rectangles; col ++) {
      uint32_t color = 0;
      if (bytes_per_pixel == 1) {  // indexed modes
        color = (first + (row * no_rectangles + col) * step) % (1 << vbe_m_info->BitsPerPixel);
      } else {  // direct color modes
        uint8_t red_first = (first & (bit_range(vbe_m_info->RedFieldPosition, vbe_m_info->RedFieldPosition+vbe_m_info->RedMaskSize))) >> vbe_m_info->RedFieldPosition;
        uint8_t green_first = (first & (bit_range(vbe_m_info->GreenFieldPosition, vbe_m_info->GreenFieldPosition+vbe_m_info->GreenMaskSize))) >> vbe_m_info->GreenFieldPosition;
        uint8_t blue_first = (first & (bit_range(vbe_m_info->BlueFieldPosition, vbe_m_info->BlueFieldPosition+vbe_m_info->BlueMaskSize))) >> vbe_m_info->BlueFieldPosition;

        uint8_t red = (red_first + col * step) % (1 << vbe_m_info->RedMaskSize);
        uint8_t green = (green_first + row * step) % (1 << vbe_m_info->GreenMaskSize);
        uint8_t blue = (blue_first + (col + row) * step) % (1 << vbe_m_info->BlueMaskSize);

        color = (red << vbe_m_info->RedFieldPosition) | (green << vbe_m_info->GreenFieldPosition)  | (blue << vbe_m_info->BlueFieldPosition);

      }
      vg_draw_rectangle(col*width, row*height, width, height, color);
    }
  }
  return OK;
}

int (vg_draw_pixmap)(uint8_t* pixel_map, xpm_image_t* img, uint16_t x, uint16_t y) {
  for (uint16_t i = 0; i < img->width; i ++) {
    for (uint16_t j = 0; j < img->height; j ++) {
      vg_draw_pixel(x + i, y + j, pixel_map[i + j * img->width]);
    }
  }
  return OK;
}
