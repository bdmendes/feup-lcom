// LCOM specific
#include <lcom/lab5.h>
#include <lcom/lcf.h>

#include "uart.h"

#include <stdint.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/lab5_rewrite/trace.txt");
  lcf_log_output("/home/lcom/labs/lab5_rewrite/output.txt");
  if (lcf_start(argc, argv))
    return 1;
  lcf_cleanup();
  return 0;
}

/* Since there is no lab 7 in lcf, using lab5 functions to test */
/* As an alternative, build a regular executable and grant special permissions
 */

int(video_test_controller)() {
  /* Read config */
  struct uart_config_t uart_config;
  bzero(&uart_config, sizeof uart_config);
  read_uart_config(&uart_config);
  printf("UART CONFIG\n");
  printf("bits per char: %d\n", uart_config.no_bits_per_char);
  printf("parity: %d\n", uart_config.parity);
  printf("stop bits: %d\n", uart_config.no_stop_bits);
  printf("bitrate: %lu\n", uart_config.bitrate);

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) { return 1; }

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width,
                          uint16_t height, uint32_t color) {
  return 1;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf,
                     uint16_t yf, int16_t speed, uint8_t fr_rate) {
  return 1;
}
