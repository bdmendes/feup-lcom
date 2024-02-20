#include "../common/lib.h"
#include "graphics.h"
#include "kbc.h"

void kbd_ih() {
  uint8_t key;
  util_sys_inb(KBC_OUT_BUF, &key);

  if (key == KBD_ESC_KEY) {
    exit_interrupt_loop();
  }
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  vg_set_mode(mode);
  sleep(delay);
  vg_exit();
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width,
                          uint16_t height, uint32_t color) {
  map_vram(mode);
  vg_set_mode(mode);
  for (int curr_y = y; curr_y < y + height; curr_y++) {
    for (int curr_x = x; curr_x < x + width; curr_x++) {
      draw_pixel(color, curr_x, curr_y);
    }
  }

  subscribe_device_interrupts(1, IRQ_EXCLUSIVE | IRQ_REENABLE, kbd_ih);
  interrupt_loop();
  unsubscribe_device_interrupts(1);

  vg_exit();

  return 0;
}
