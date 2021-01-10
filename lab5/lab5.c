// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

#include "my_utils.h"
#include "graphics.h"
#include "kbc.h"
#include "i8254.h"

extern uint8_t buffer_data; // keyboard data
extern int timer_counter;

// Any header files included below this line should have been created by you

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
  set_graphics_mode(mode);
  delay_seconds(delay);
  return vg_exit();
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
uint16_t width, uint16_t height, uint32_t color) {
  if (map_vram_memory(mode) != OK){
    printf("error mapping memory for the desired mode!");
    return !OK;
  }

  if (set_graphics_mode(mode) != OK){
    printf("error setting up graphics mode");
    return !OK;
  }

  vg_draw_rectangle(x,y,width,height,color);
  wait_while_not_key(ESC_BREAKCODE);
  return vg_exit();
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {

  if (map_vram_memory(mode) != OK){
    printf("error mapping memory for the desired mode!");
    return !OK;
  }

  if (set_graphics_mode(mode) != OK){
    printf("error setting up graphics mode");
    return !OK;
  }
  
  vg_draw_matrix(mode,no_rectangles,first,step);
  wait_while_not_key(ESC_BREAKCODE);
  return vg_exit();
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  if (map_vram_memory(GRAPHICS_MODE_1024x768_INDEXED) != OK){
    printf("error mapping memory for the desired mode!");
    return !OK;
  }

  if (set_graphics_mode(GRAPHICS_MODE_1024x768_INDEXED) != OK){
    printf("error setting up graphics mode");
    return !OK;
  }

  xpm_image_t img;
  uint8_t* map = xpm_load(xpm, XPM_INDEXED, &img);
  vg_draw_pixmap(map, x, y, img);
  
  wait_while_not_key(ESC_BREAKCODE);
  return vg_exit();
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {

  if (map_vram_memory(GRAPHICS_MODE_1024x768_INDEXED) != OK){
    printf("error mapping memory for the desired mode!");
    return !OK;
  }

  if (set_graphics_mode(GRAPHICS_MODE_1024x768_INDEXED) != OK){
    printf("error setting up graphics mode");
    return !OK;
  }

  xpm_image_t img;
  uint8_t* map = xpm_load(xpm, XPM_INDEXED, &img);
  vg_draw_pixmap(map, xi, yi, img);

  /*whether to move horizontally or vertically*/
  int16_t x_movement = (xf == xi) ? 0 : ((xf>xi)? 1: -1);
  int16_t y_movement = (yf == yi) ? 0 : ((yf>yi)? 1: -1);

  /*move until it is on the start position, or esc received*/
  int r, ipc_status;
  message msg;
  uint8_t timer_bit_no = TIMER0_IRQ;
  uint8_t kbd_bit_no = KDB_IRQ;
  timer_subscribe_int(&timer_bit_no);
  kbd_subscribe_int(&kbd_bit_no);
 
  for (uint16_t frames_left = -speed;;) {
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      if (_ENDPOINT_P(msg.m_source == HARDWARE)) {

        if (msg.m_notify.interrupts & BIT(kbd_bit_no)) {
          kbd_ih();
          if (buffer_data == ESC_BREAKCODE) break;
        }

        if (msg.m_notify.interrupts & BIT(timer_bit_no)){
          timer_int_handler();
          if ( (xi == xf && yi == yf) || timer_counter % (60/fr_rate) != 0) continue;
          if (speed < 0) {
            frames_left--;
            if (frames_left == 0){
              frames_left = -speed;
              xi += x_movement;
              yi += y_movement;
            }
            else continue;
          }
          else if (speed > 0){
            xi += x_movement*speed;
            yi += y_movement*speed;
            if ( (x_movement>0 && xi > xf) || (x_movement<0 && xi<xf) ) xi = xf;
            else if ( (y_movement>0 && yi>yf) || (y_movement<0 && yi<yf)) yi = yf;
          }
          vg_clear_screen();
          vg_draw_pixmap(map, xi, yi, img);
        }

      }
    }
  }

  kbd_unsubscribe_int();
  timer_unsubscribe_int();
  return vg_exit();
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
