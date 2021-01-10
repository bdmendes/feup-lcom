#include "vbe.h"

static char* video_buf[3] = {NULL, NULL, NULL};
static uint8_t current_buf = 0;
static uint32_t buf_size = 0;
static uint8_t bytes_per_pixel = 0;
static vbe_mode_info_t mode_info;
static bool has_updates[3] = {true, true, true};

/**
 * @brief  sets the graphics card mode
 * @param  mode: the desired mode
 * @retval OK if success, otherwise !OK
 */
static int (set_graphics_mode)(uint16_t mode);

/**
 * @brief  maps the vram according to the graphics mode
 * @param  mode: the graphics card mode
 * @retval OK if success, otherwise !OK
 */
static int (map_vram_memory)(uint16_t mode);

/**
 * @brief  clears the current video buffer
 * @retval None
 */
static void (vg_clear_current_buffer)();

/**
 * @brief  gets the graphics mode information
 * @param  mode: the mode
 * @param  mode_info: the information read  
 * @retval OK if success, otherwise !OK
 */
static int (vbe_get_mode_info_ours)(uint16_t mode, vbe_mode_info_t* mode_info);

int (vg_switch_mode)(uint16_t mode){
  return set_graphics_mode(mode) & map_vram_memory(mode);
}

static int (set_graphics_mode)(uint16_t mode) {
  reg86_t reg;
  memset(&reg, 0, sizeof(reg));
  reg.ah = AH_INPUT_FUNCTION;
  reg.al = AL_INPUT_SET_MODE;
  reg.bx = mode | GRAPHICS_LINEAR_MODE_BIT;
  reg.intno = BIOS_VIDEO_CARD;
  
  if (sys_int86(&reg) != OK) {
    printf("set_vbe_mode: sys_int86() failed\n");
    return !OK;
  }
  if (reg.al != AL_OUTPUT_FUNCTION_CALL_SUCCEEDED || reg.ah != AH_OUTPUT_NO_ERRORS){
    printf("couldnt set graphics mode!");
    return !OK;
  }
  return OK;
}

static int (map_vram_memory)(uint16_t mode){

  memset(&mode_info, 0, sizeof(mode_info));

  if (vbe_get_mode_info_ours(mode, &mode_info) != OK){
    printf("could not get mode info\n");
    return !OK;
  }

  if (!(mode_info.ModeAttributes & BIT(0))){
    printf("mode not supported by hardware!");
    return !OK;
  }

  if (!(mode_info.ModeAttributes & BIT(7))){
    printf("linear buffer mode is not available!");
    return !OK;
  }

  int r;
  struct minix_mem_range mr;
  unsigned int vram_base = mode_info.PhysBasePtr;
  buf_size = mode_info.YResolution * mode_info.BytesPerScanLine;
  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + 3 * buf_size;

  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))){
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);
  }
  
  /* map three screens for triple buffering */
  for (uint8_t buf = 0; buf < 3; ++buf){
    video_buf[buf] = vm_map_phys(SELF, (void*) (phys_bytes) (mr.mr_base + buf*buf_size), buf_size);
    if(video_buf[buf] == MAP_FAILED){
      panic("couldnt map video memory");
      return !OK;
    }
    else {
      current_buf = buf;
      vg_clear_current_buffer(); // make sure there are no traces from previous executions
    }
    has_updates[buf] = true;
  }

  current_buf = 1;
  bytes_per_pixel = mode_info.BytesPerScanLine / mode_info.XResolution;
  return OK;
}

static int (vbe_get_mode_info_ours)(uint16_t mode, vbe_mode_info_t* mode_info){
  reg86_t reg;
  memset(&reg, 0, sizeof(reg));
  mmap_t map;

  if (lm_alloc(sizeof(vbe_mode_info_t), &map) == NULL){
    printf("error allocating vbe_mode_info_t memory block");
    return !OK;
  }

  reg.ah = AH_INPUT_FUNCTION;
  reg.al = AL_INPUT_GET_MODE_INFO;
  reg.es = PB2BASE(map.phys);
  reg.di = PB2OFF(map.phys);
  reg.cx = mode;
  reg.intno = BIOS_VIDEO_CARD;

  if (sys_int86(&reg) != OK) {
    printf("get_mode_info_ours: sys_int86() failed");
    lm_free(&map);
    return !OK;
  }
  if (reg.al != AL_OUTPUT_FUNCTION_CALL_SUCCEEDED || reg.ah != AH_OUTPUT_NO_ERRORS){
    printf("couldnt get vbe status!");
    lm_free(&map);
    return !OK;
  }

  *mode_info = *((vbe_mode_info_t*)map.virt);
  lm_free(&map);

  return OK;
}

int (vg_flip_page)(){
  reg86_t reg;
  memset(&reg, 0, sizeof(reg));

  reg.ah = AH_INPUT_FUNCTION;
  reg.al = AL_INPUT_SET_DISPLAY_START;
  reg.bl = BL_INPUT_SET_DISPLAY_START;
  reg.cx = 0;
  reg.dx = current_buf * mode_info.YResolution;
  reg.intno = BIOS_VIDEO_CARD;

  if (sys_int86(&reg) != OK) {
    printf("set_display_start: sys_int86() failed");
    return !OK;
  }
  if (reg.al != AL_OUTPUT_FUNCTION_CALL_SUCCEEDED || reg.ah != AH_OUTPUT_NO_ERRORS){
    printf("couldnt set display start!");
    return !OK;
  }

  has_updates[current_buf] = false;
  current_buf = (current_buf + 1) % 3;
  return OK;
}

static inline void (vg_clear_current_buffer)(){
  memset(get_current_video_buf(),0,buf_size);
}

inline vbe_mode_info_t* (get_mode_info)(){
  return &mode_info;
}

inline uint8_t (get_bytes_per_pixel)(){
  return bytes_per_pixel;
}

inline char* (get_current_video_buf)(){
  return video_buf[current_buf];
}

inline void (draw_pixel)(char* buffer, uint16_t x, uint16_t y, uint32_t color){
  if (color == TRANSPARENCY_COLOR_8_8_8_8) return;
  memcpy(buffer + (y*get_mode_info()->XResolution + x)*get_bytes_per_pixel(), &color, get_bytes_per_pixel());
}

inline void (vg_signal_update)(){
  for (uint8_t buf = 0; buf < 3; ++buf) has_updates[buf] = true;
}

inline bool (vg_has_updates)(){
  for (uint8_t buf = 0; buf < 3; ++buf) if (has_updates[buf]) return true;
  return false;
}
