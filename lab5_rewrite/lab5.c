// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "graphics.h"
#include "my_utils.h"
#include "keyboard.h"
#include "timer.h"


int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5_rewrite/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5_rewrite/output.txt");

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

  if (vbe_set_mode(mode) != OK){
    printf("error setting mode\n");
    return !OK;
  }
  delay_seconds(delay);
  return vg_exit();
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {


  // map the video memory to the process' address
  vbe_map_vram(mode);

  // change the video mode to the specified in the argument
  if (vbe_set_mode(mode) != OK){
    return !OK;
  }


  // draw a rectangle top left vertix on (x, y)
  vg_draw_rectangle(x, y, width, height, color);

  int r;
  int ipc_status;
  message msg;

  uint8_t buffer_data = get_buffer_data();

  uint8_t kbd_bit_no = KDB_IRQ;
  kbd_subscribe_int(&kbd_bit_no);
  

  // leave on ESC
  while( buffer_data != ESC_BREAKCODE ){
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ){
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /*received notification*/
      if (_ENDPOINT_P(msg.m_source) == HARDWARE) {
        if (msg.m_notify.interrupts & BIT(kbd_bit_no)) { /*subscribed interrupt*/
          kbd_ih();
          buffer_data = get_buffer_data();
        }
      }
    }    
  }

  if (kbd_unsubscribe_int() != OK){
    printf("error unsubscribing kbd int");
    return !OK;
  }

  // reset mode
  vg_exit(); 

  return OK;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {

  // map the video memory to the process' address
  vbe_map_vram(mode);
  
  // change the video mode
  vbe_set_mode(mode);
  // draw a pattern of colored rectangles
  vg_draw_matrix(no_rectangles, first, step);
  // leave on esc
  int r;
  int ipc_status;
  message msg;

  uint8_t buffer_data = get_buffer_data();

  uint8_t bit_no = KDB_IRQ;
  kbd_subscribe_int(&bit_no);

  while( buffer_data != ESC_BREAKCODE ){
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ){
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /*received notification*/
      if (_ENDPOINT_P(msg.m_source) == HARDWARE) {
        if (msg.m_notify.interrupts & BIT(bit_no)) { /*subscribed interrupt*/
          kbd_ih();
          buffer_data = get_buffer_data();
        }
      }
    }    
  }

  if (kbd_unsubscribe_int() != OK){
    printf("error unsubscribing kbd int");
    return !OK;
  }

  // reset mode
  vg_exit(); 


  return OK;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {

  // map the video memory to the process' address
  vbe_map_vram(VBE_GRAPHIC_1024_768_256);
  
  // change the video mode
  vbe_set_mode(VBE_GRAPHIC_1024_768_256);

  xpm_image_t img;
  uint8_t* pixel_map = xpm_load(xpm, XPM_INDEXED, &img); 
  
  // use vbe mode 105 to display xpm at coords(x, y)
  vg_draw_pixmap(pixel_map, &img, x, y);
  // convert the xpm to pixmap using xpm_load

  // leave on esc
  int r;
  int ipc_status;
  message msg;

  uint8_t buffer_data = get_buffer_data();

  uint8_t bit_no = KDB_IRQ;
  kbd_subscribe_int(&bit_no);

  while( buffer_data != ESC_BREAKCODE ){
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ){
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /*received notification*/
      if (_ENDPOINT_P(msg.m_source) == HARDWARE) {
        if (msg.m_notify.interrupts & BIT(bit_no)) { /*subscribed interrupt*/
          kbd_ih();
          buffer_data = get_buffer_data();
        }
      }
    }    
  }

  if (kbd_unsubscribe_int() != OK){
    printf("error unsubscribing kbd int");
    return !OK;
  }


  vg_exit();
  

  return OK;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate) {
  
  if (vbe_map_vram(VBE_GRAPHIC_1024_768_256) != OK) {
    printf("Error during vram mapping\n");
    return !OK;
  }

  if (vbe_set_mode(VBE_GRAPHIC_1024_768_256) != OK) {
    printf("Error seting vbe mode\n");
    return !OK;
  }

  xpm_image_t img;
  uint8_t* pixel_map = xpm_load(xpm, XPM_INDEXED, &img); 
  
  // use vbe mode 105 to display xpm at coords(xi, yi)
  vg_draw_pixmap(pixel_map, &img, xi, yi);

  uint8_t x_dir = (xf > xi) ? 1 : -1;
  uint8_t y_dir = (yf > yi) ? 1 : -1;

  int r;
  int ipc_status;
  message msg;

  uint8_t kbd_bit_no = KDB_IRQ;
  kbd_subscribe_int(&kbd_bit_no);

  uint8_t timer_bit_no = TIMER0_IRQ;
  timer_subscribe_int(&timer_bit_no);
  uint8_t buffer_data = 0;

  for ( uint16_t frames_left = -speed;; ){
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ){
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /*received notification*/
      if (_ENDPOINT_P(msg.m_source) == HARDWARE) {
        if (msg.m_notify.interrupts & BIT(kbd_bit_no)) { /*subscribed interrupt*/
          kbd_ih();
          buffer_data = get_buffer_data();
          if (buffer_data == ESC_BREAKCODE) {
            break;
          }
        }
      }
      if (_ENDPOINT_P(msg.m_source) == HARDWARE) {
        if (msg.m_notify.interrupts & BIT(timer_bit_no)) { /*subscribed interrupt*/
          timer_int_handler();
          if (!(xi == xf && yi == yf) && get_timer_count() % (60 / fr_rate) == 0) {
            if (speed > 0) {
              xi += x_dir*speed;
              yi += y_dir*speed;
              if ((x_dir > 0 && xi > xf) || (x_dir < 0 && xi < xf)) xi = xf;
              if ((y_dir > 0 && yi > yf) || (y_dir < 0 && yi < yf)) yi = yf;
            }
            else if (speed < 0) {
              frames_left --;
              
              if (frames_left == 0) {
                frames_left = -speed;
                xi += x_dir;
                yi += y_dir;
              
              } else continue;
            }
            
          }
          
          vg_draw_pixmap(pixel_map, &img, xi, yi);
        }
      }
    }    
  }


  if (kbd_unsubscribe_int() != OK){
    printf("error unsubscribing kbd int");
    return !OK;
  }

  if (timer_unsubscribe_int() != OK) {
    printf("error unsubscribing timer int\n");
    return !OK;
  }


  vg_exit();


  return OK;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
