// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <lcom/vbe.h>
#include <lcom/video_gr.h>
#include <minix/com.h>
#include <minix/endpoint.h>
#include <minix/syslib.h>
#include <minix/vm.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");
  lcf_log_output("/home/lcom/labs/lab5/output.txt");
  if (lcf_start(argc, argv))
    return 1;
  return lcf_cleanup();
}

typedef struct {
  vbe_mode_info_t mode_info;
  char *mem;
} g_config;

static void set_mode(uint16_t mode) {
  struct reg86 r;
  memset(&r, 0, sizeof r);
  r.ah = 0x4f;
  r.al = 0x02;
  r.bx = mode | BIT(14);
  r.intno = 0x10;
  if (sys_int86(&r) != OK) {
    perror("sys_int86 failed");
  }
}

static g_config map_mem(uint16_t mode) {
  vbe_mode_info_t info;
  if (vbe_get_mode_info(mode, &info) != OK) {
    printf("vbe_get_mode_info failed");
    return (g_config){info, NULL};
  }

  struct minix_mem_range mr;
  size_t size = info.YResolution * info.BytesPerScanLine;
  mr.mr_base = (phys_bytes) info.PhysBasePtr;
  mr.mr_limit = mr.mr_base + size;

  if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, (void *)&mr) != OK) {
    printf("sys_privctl failed");
    return (g_config){info, NULL};
  }

  void *addr = vm_map_phys(SELF, (void *)info.PhysBasePtr,
                           info.YResolution * info.BytesPerScanLine);

  if (addr == NULL) {
      printf("addr is NULL!");
      return (g_config){info, NULL};
  }

  return (g_config){info, (char *)addr};
}

static void draw_pixel(g_config *conf, uint16_t x, uint16_t y, uint32_t color) {
  size_t s = (conf->mode_info.BitsPerPixel + 7) / 8; // cache this!
  if (x >= conf->mode_info.XResolution || y >= conf->mode_info.YResolution) return;
  char *addr = conf->mem + y * conf->mode_info.BytesPerScanLine + x * s;
  memcpy(addr, &color, s);
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  set_mode(mode);
  sleep(delay);
  return vg_exit();
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width,
                          uint16_t height, uint32_t color) {
  g_config conf = map_mem(0x115);
  if (conf.mem == NULL) {
      return 1;
  }
  set_mode(0x115);

  for (uint16_t yy = y; yy < y + height; yy++) {
      for (uint16_t xx = x; xx < x + width; xx++) {
          draw_pixel(&conf, xx, yy, color);
      }
  }

  // Not what the lab wants, but I want to remember this :)
  sleep(3);
  return vg_exit();
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  printf("%s(%8p, %u, %u): under construction\n", __func__, xpm, x, y);
  return 1;
}
