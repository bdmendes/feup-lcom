#include "graphics.h"
#include <lcom/lab5.h>
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  set_video_mode(mode);
  sleep(delay);
  return vg_exit();
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width,
                          uint16_t height, uint32_t color) {
  if (map_graphics_vram(mode) != 0) {
    return 1;
  }

  if (set_video_mode(mode) != 0) {
    return 1;
  }

  for (int i = x; i < x + width; i++) {
    for (int j = y; j < y + height; j++) {
      draw_pixel(i, j, color);
    }
  }

  sleep(5);
  return vg_exit();
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first,
                        uint8_t step) {
  printf("Not implemented yet!\n");
  return 1;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  printf("Not implemented yet!\n");
  return 1;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf,
                     uint16_t yf, int16_t speed, uint8_t fr_rate) {
  printf("Not implemented yet!\n");
  return 1;
}

int(video_test_controller)() {
  printf("Not implemented yet!\n");
  return 1;
}
