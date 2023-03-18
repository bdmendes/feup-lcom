#include "graphics.h"
#include <machine/int86.h>

static uint8_t *video_mem;
static vbe_mode_info_t mode_info;

int set_video_mode(uint16_t mode) {
  struct reg86 args;
  memset(&args, 0, sizeof(args));

  args.ah = VBE_FUNCTION;
  args.al = VBE_SET_MODE;
  args.bx = BIT(14) | mode;
  args.intno = VBE_VIDEO_SERVICE;

  if (sys_int86(&args) != OK) {
    printf("set_video_mode: sys_int86() failed \n");
    return 1;
  }
  return 0;
}

int map_graphics_vram(uint16_t mode) {
  if (vbe_get_mode_info(mode, &mode_info) != 0) {
    printf("map_graphics_vram: vbe_get_mode_info() failed \n");
    return 1;
  }

  struct minix_mem_range mr;
  memset(&mr, 0, sizeof(mr));

  mr.mr_base = (phys_bytes)mode_info.PhysBasePtr;
  mr.mr_limit = mr.mr_base + mode_info.XResolution * mode_info.YResolution *
                                 mode_info.BytesPerScanLine /
                                 mode_info.XResolution;

  if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr) != OK) {
    printf("map_graphics_vram: sys_privctl() failed \n");
    return 1;
  }

  video_mem = (uint8_t *)vm_map_phys(SELF, (void *)mr.mr_base,
                                     mr.mr_limit - mr.mr_base);

  if (video_mem == MAP_FAILED) {
    printf("map_graphics_vram: vm_map_phys() failed \n");
    return 1;
  }

  return 0;
}

int draw_pixel(uint16_t x, uint16_t y, uint32_t color) {
  if (x >= mode_info.XResolution || y >= mode_info.YResolution) {
    printf("draw_pixel: invalid coordinates \n");
    return 1;
  }

  uint8_t *pixel = video_mem + y * mode_info.BytesPerScanLine +
                   x * (mode_info.BytesPerScanLine / mode_info.XResolution);
  memcpy(pixel, &color, mode_info.BitsPerPixel / 8);
  return 0;
}
