#include <lcom/lcf.h>
#include <lcom/proj.h>

#include "lib/devices/graphics/vbe.h"
#include "lib/devices/kbc/keyboard.h"
#include "lib/devices/kbc/mouse.h"
#include "lib/devices/rtc/rtc.h"
#include "lib/devices/timer/timer.h"

#include "resources.h"
#include "ev_handler.h"

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");
  lcf_log_output("/home/lcom/labs/proj/output.txt");
  if (lcf_start(argc, argv) != OK) return !OK;
  return lcf_cleanup();
}

int (proj_main_loop)(int argc, char *argv[]) {

  if (vg_switch_mode(GRAPHICS_MODE_DIRECT_32BIT) != OK){
    return !OK;
  }
  
  Resources* resources = create_game_resources();
  if (resources == NULL) {
    vg_exit();
    printf("game loading failed!\n");
    return !OK;
  }

  APP_STATE app_state = MAIN_MENU;
  message msg;
  uint8_t kbd_bit_no = KDB_IRQ, timer_bit_no = TIMER0_IRQ, mouse_bit_no = MOUSE_IRQ, rtc_bit_no = RTC_IRQ;
  rtc_set_alarm_every_second();
  timer_subscribe_int(&timer_bit_no);
  kbd_subscribe_int(&kbd_bit_no);
  rtc_subscribe_int(&rtc_bit_no);
  rtc_disable_all_interrupts();
  rtc_enable_alarm_interrupts();
  m_mouse_enable_data_reporting();
  mouse_subscribe_int(&mouse_bit_no);
 
  for (int err, ipc_status;;) {
    if ( (err = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
      printf("driver_receive failed with: %d", err);
      continue;
    }
    if (is_ipc_notify(ipc_status) && _ENDPOINT_P(msg.m_source == HARDWARE)) {

      if (msg.m_notify.interrupts & BIT(mouse_bit_no)){
        mouse_ih();
        if (mouse_packet_is_ready()){
          struct packet pp = get_mouse_packet();
          move_sprite(resources->mouse,pp.delta_x,-pp.delta_y,false, true);
          switch(app_state){
            case MAIN_MENU: case INSTRUCTIONS: case SETTINGS:
              app_state = handle_main_menu(resources->game, resources->menu, MOUSE, &pp, 0, app_state);
              break;
            case IN_GAME:
              app_state = handle_in_game(resources->game, MOUSE, &pp, 0);
              break;
            case IN_GAME_END_MESSAGE: case IN_GAME_CLUE:
              app_state = handle_in_game_panel(resources->game, MOUSE, &pp, 0, app_state);
            default:
              break;
          }
        }
      }

      if (msg.m_notify.interrupts & BIT(rtc_bit_no)) {
        rtc_ih();
        switch (app_state){
          case RTC:
            app_state = handle_in_game(resources->game, RTC, NULL, 0);
            break;
          default:
            break;
        }
      }

      if (msg.m_notify.interrupts & BIT(kbd_bit_no)) {
        kbd_ih();
        uint8_t scan_code = get_buffer_data();
        if (scan_code != FIRST_OF_TWO_BYTES) switch (app_state){
          case MAIN_MENU: case INSTRUCTIONS: case SETTINGS:
            app_state = handle_main_menu(resources->game, resources->menu, KEYBOARD, NULL, scan_code, app_state);
            break;
          case IN_GAME:
            app_state = handle_in_game(resources->game, KEYBOARD, NULL, scan_code);
            break;
          case IN_GAME_END_MESSAGE: case IN_GAME_CLUE:
            app_state = handle_in_game_panel(resources->game, KEYBOARD, NULL, scan_code, app_state);
            break;
          default:
            break;
        }
      }

      if (msg.m_notify.interrupts & BIT(timer_bit_no)){
        timer_int_handler();
        if (vg_has_updates()){
          switch (app_state){
            case MAIN_MENU: case INSTRUCTIONS: case SETTINGS:
              app_state = handle_main_menu(resources->game, resources->menu, TIMER, NULL, 0, app_state);
              break;
            case IN_GAME:
              app_state = handle_in_game(resources->game, TIMER, NULL, 0);
              break;
            case IN_GAME_END_MESSAGE: case IN_GAME_CLUE:
              app_state = handle_in_game_panel(resources->game, TIMER, NULL, 0, app_state);
            default:
              break;
          }
          draw_sprite(get_current_video_buf(), resources->mouse);
          vg_flip_page();
        }
      }

      if (app_state == EXIT) break;
    }
  }

  mouse_unsubscribe_int();
  mouse_disable_data_reporting();
  timer_unsubscribe_int();
  kbd_unsubscribe_int();
  rtc_disable_all_interrupts();
  rtc_unsubscribe_int();

  vg_exit();
  destroy_game_resources(resources);
  
  return OK;
}
