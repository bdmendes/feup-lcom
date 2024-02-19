#include "vbe.h"

static void* video_mem = 0; /* frame-buffer VM address */
static vbe_mode_info_t vbe_m_info;

int vbe_set_mode(uint16_t mode) {

  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86_t));

  reg86.ah = VBE_FUNCTION_AH;
  reg86.al = VBE_SET_MODE;
  reg86.bx = mode | VBE_LINEAR_FB;
  reg86.intno = BIOS_VIDEO_CARD;

  if(sys_int86(&reg86) != OK){
    printf("set_vbe_mode: sys_int86() failed\n");
    return !OK;
  }

  return OK;
}

int vbe_map_vram(uint16_t mode) {

  struct minix_mem_range mr;
  unsigned int vram_base;  /* VRAM's physical addresss */
  unsigned int vram_size;  /* VRAM's size */
  int r;

  /* Use VBE function 0x01 to initialize vram_base and vram_size */

  memset(&vbe_m_info, 0, sizeof(vbe_mode_info_t));
  if (m_vbe_get_mode_info(mode, &vbe_m_info) != OK) {
    printf("Invalid mode\n");
    return !OK;
  }
  vram_base = vbe_m_info.PhysBasePtr;
  vram_size = vbe_m_info.BitsPerPixel == 15 ? vbe_m_info.XResolution * vbe_m_info.YResolution * vbe_m_info.BitsPerPixel : vbe_m_info.XResolution * vbe_m_info.YResolution * (vbe_m_info.BitsPerPixel/8);

  /* Allow memory mapping */
  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);
  }

  /* Map memory */
  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

  if (video_mem == MAP_FAILED) {
    panic("couldn't map video memory");
  }

  return OK;
}

void* (vbe_get_video_mem)() {
  return video_mem;
}

vbe_mode_info_t* (vbe_get_vbe_m_info)(){
  return &vbe_m_info;
}

int (m_vbe_get_mode_info)(uint16_t mode, vbe_mode_info_t* vmi_p) {
  reg86_t reg86;
  memset(&reg86, 0, sizeof(reg86_t));
  mmap_t mmap;
  memset(&mmap, 0, sizeof(mmap_t));

  if (lm_alloc(sizeof(mmap_t), &mmap) == 0) {
    printf("error allocating memory block\n");
    return !OK;
  }

  phys_bytes buff = mmap.phys;

  reg86.ah = VBE_FUNCTION_AH;
  reg86.al = VBE_GET_MODE_INFO;

  reg86.es = PB2BASE(buff);
  reg86.di = PB2OFF(buff);
  reg86.cx = mode;
  reg86.intno = BIOS_VIDEO_CARD;
  if (sys_int86(&reg86) != OK) {
    printf("error making bios call\n");
    return !OK;
  }
  if (reg86.ax != VBE_FUNCTION_AH) {
    printf("error getting mode info\n");
    lm_free(&mmap);
    return !OK;
  }

  *vmi_p = *((vbe_mode_info_t*)mmap.virt);
  lm_free(&mmap);

  if (!(vmi_p->ModeAttributes & VBE_MODE_SUPPORTED)) {
    printf("Mode not supported by hardware\n");
    return !OK;
  }

  if (!(vmi_p->ModeAttributes & VBE_LINEAR_BUFFER_AVAILABLE)) {
    printf("Linear mode is not available\n");
    return !OK;
  } 

  return OK;
}
