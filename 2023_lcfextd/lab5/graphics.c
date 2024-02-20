#include "graphics.h"
#include <lcom/lcf.h>

static vbe_mode_info_t mode_info;
static char *video_mem;

void vg_set_mode(int mode) {
  struct reg86 r86;

  memset(&r86, 0, sizeof(r86)); /* zero the structure */

  r86.intno = VBE_GRAPHICS_INTNO;
  r86.ah = VBE_GRAPHICS_FUNCTION_AH;
  r86.al = VBE_SET_MODE_AL;
  r86.bx = mode | BIT(14);

  sys_int86(&r86);
}

void map_vram(int mode) {
  vbe_get_mode_info(mode, &mode_info);

  struct minix_mem_range mr;
  unsigned int vram_base = mode_info.PhysBasePtr;
  unsigned int vram_size = mode_info.BytesPerScanLine * mode_info.YResolution;

  /* Allow memory mapping */
  mr.mr_base = (phys_bytes)vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  int r;
  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  video_mem = (char *)vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
  if (video_mem == MAP_FAILED)
    panic("couldn’t map video memory");
}

void draw_pixel(uint32_t color, uint16_t x, uint16_t y) {
  int bytes_per_pixel = mode_info.BytesPerScanLine / mode_info.XResolution;
  char *pos =
      video_mem + (mode_info.BytesPerScanLine) * y + bytes_per_pixel * x;
  memcpy(pos, &color, bytes_per_pixel);
}