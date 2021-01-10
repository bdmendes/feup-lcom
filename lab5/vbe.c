#include "vbe.h"

#include <math.h>

static vbe_mode_info_t mode_info;
static void* video_mem = NULL;

int (set_graphics_mode)(uint16_t mode) {
  reg86_t reg;
  memset(&reg, 0, sizeof(reg));
  reg.ah = AH_FUNCTION;
  reg.al = AL_VBE_SET_MODE;
  reg.bx = mode | GRAPHICS_LINEAR_MODE_BIT;
  reg.intno = BIOS_VIDEO_CARD;
  if (sys_int86(&reg) != OK) {
    printf("set_vbe_mode: sys_int86() failed\n");
    return !OK;
  }
  return OK;
}

int (map_vram_memory)(uint16_t mode){
  memset(&mode_info, 0, sizeof(mode_info));

  if (vbe_get_mode_info_ours(mode,&mode_info) != OK){
    return !OK;
  }

  int r;
  struct minix_mem_range mr;
  unsigned int vram_base = mode_info.PhysBasePtr;
  unsigned int vram_size = mode_info.XResolution * mode_info.YResolution * mode_info.BitsPerPixel;
  
  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;
  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))){
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);
  }
    
  video_mem = vm_map_phys(SELF, (void*)mr.mr_base, vram_size);

  if(video_mem == MAP_FAILED){
    panic("couldnt map video memory");
    return !OK;
  }

  return OK;
}

int (vbe_get_mode_info_ours)(uint16_t mode, vbe_mode_info_t* mode_info){
  reg86_t reg;
  memset(&reg, 0, sizeof(reg));
  mmap_t map;

  if (lm_alloc(sizeof(vbe_mode_info_t), &map) == NULL){
    printf("error allocating vbe_mode_info_t memory block in real mode");
    return !OK;
  }

  phys_bytes buf = map.phys;

  reg.ah = AH_FUNCTION;
  reg.al = AL_VBE_RETURN_MODE_INFO;
  reg.es = PB2BASE(buf);
  reg.di = PB2OFF(buf);
  reg.cx = mode;
  reg.intno = BIOS_VIDEO_CARD;

  if (sys_int86(&reg) != OK) {
    printf("get_mode_info_ours: sys_int86() failed");
    lm_free(&map);
    return !OK;
  }

  if (reg.ax != AH_FUNCTION){
    printf("couldnt get vbe status!");
    lm_free(&map);
    return !OK;
  }

  *mode_info = *((vbe_mode_info_t*)map.virt);
  lm_free(&map);

  if (!(mode_info->ModeAttributes & BIT(0))){
    printf("mode not supported by hardware!");
    return !OK;
  }

  if (!(mode_info->ModeAttributes & BIT(7))){
    printf("linear buffer mode is not available!");
    return !OK;
  }

  return OK;
}

vbe_mode_info_t* (get_mode_info)(){
  return &mode_info;
}

void* (get_video_mem)(){
  return video_mem;
}
