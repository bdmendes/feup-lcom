#include "asprite.h"
#include <stdarg.h>

/**
 * @brief  makes the animated sprite show the next sprite of the animation
 * @param  fig: the animated sprite
 * @retval None
 */
static void (asprite_switch_sprite)(AnimSprite *fig);

AnimSprite* (create_asprite)(uint16_t x, uint16_t y, uint8_t frames_per_map, uint8_t no_pic, const char *picture1[], ...) {

  AnimSprite *asp = malloc(sizeof(AnimSprite));
  if (asp == NULL) return NULL;

  asp->cur_sp = create_sprite(picture1, x, y);
  if (asp->cur_sp == NULL){
    return NULL;
  }

  /* assemble sprite bitmaps including mirrors */
  asp->map = malloc(no_pic * sizeof(char *));
  asp->mirror_map = malloc(no_pic * sizeof(char *));
  if (asp->map == NULL || asp->mirror_map == NULL) return NULL;
  asp->is_mirrored = malloc(sizeof(bool) * no_pic);
  if (asp->is_mirrored == NULL) return NULL;
  asp->is_mirrored[0] = false;
  asp->map[0] = asp->cur_sp->map;
  asp->mirror_map[0] = get_mirrored_pixmap(asp->cur_sp->map, asp->cur_sp->width, asp->cur_sp->height);

  asp->num_fig = no_pic;
  asp->cur_fig = 0;
  asp->is_idle = true;
  asp->should_mirror = false;
  asp->frames_per_map = frames_per_map;
  asp->frames_left = frames_per_map;

  va_list ap;
  va_start(ap, picture1);
  for (int i = 1; i < no_pic; i++) {
    xpm_image_t img;
    asp->map[i] = (char*) xpm_load(va_arg(ap, xpm_row_t*), XPM_8_8_8_8, &img);
    if (asp->map[i] == NULL || img.width != asp->cur_sp->width || img.height != asp->cur_sp->height) {
      printf("error parsing asprite xpm\n");
      for (int j = 1; j < i; j++) {
        free(asp->map[i]);
        asp->map[i] = NULL;
      }
      free(asp->map);
      asp->map = NULL;
      destroy_sprite(asp->cur_sp);
      free(asp);
      asp = NULL;
      va_end(ap);
      return NULL;
    }
    asp->is_mirrored[i] = false;
    asp->mirror_map[i] = get_mirrored_pixmap(asp->map[i], asp->cur_sp->width, asp->cur_sp->height);
  }
  va_end(ap);

  return asp;
}

void (animate_asprite)(AnimSprite* fig){
  if (--fig->frames_left) return;
  fig->frames_left = fig->frames_per_map;
  asprite_switch_sprite(fig);
}

static void (asprite_switch_sprite)(AnimSprite *fig) {
  fig->cur_fig = ++fig->cur_fig % fig->num_fig;
  fig->is_idle = fig->cur_fig == 0;
  fig->cur_sp->map = fig->should_mirror ? fig->mirror_map[fig->cur_fig] : fig->map[fig->cur_fig];
  vg_signal_update();
}

int (draw_asprite)(char* buffer, AnimSprite *fig) {

  if (fig->should_mirror && !fig->is_mirrored[fig->cur_fig]){
    fig->cur_sp->map = fig->mirror_map[fig->cur_fig];
    fig->is_mirrored[fig->cur_fig] = true;
  }
  else if (!fig->should_mirror && fig->is_mirrored[fig->cur_fig]){
    fig->cur_sp->map = fig->map[fig->cur_fig];
    fig->is_mirrored[fig->cur_fig] = false;
  }

  return draw_sprite(buffer, fig->cur_sp);
}

void (asprite_go_idle)(AnimSprite* fig){
  fig->cur_fig = 0;
  fig->is_idle = true;
  fig->cur_sp->map = fig->should_mirror ? fig->mirror_map[fig->cur_fig] : fig->map[fig->cur_fig];
  vg_signal_update();
}

void (destroy_asprite)(AnimSprite *fig) {
  if (fig == NULL || fig->map == NULL || fig->mirror_map == NULL) return;
  for (int i = 0; i < fig->num_fig; i++){
    free(fig->map[i]);
    fig->map[i] = NULL;
    free(fig->mirror_map[i]);
    fig->mirror_map[i] = NULL;
  }
  free(fig->map);
  fig->map = NULL;
  free(fig->mirror_map);
  fig->mirror_map = NULL;
  //destroy_sprite(fig->cur_sp);
  //free(fig->cur_sp);
  //fig->cur_sp = NULL;
  free(fig->is_mirrored);
  fig->is_mirrored = NULL;
}

int (asprite_move)(AnimSprite *fig, int16_t xmov, int16_t ymov, bool allow_image_cut_left_up, bool allow_image_cut_right_down) {
  return move_sprite(fig->cur_sp, xmov, ymov, allow_image_cut_left_up, allow_image_cut_right_down);
}
