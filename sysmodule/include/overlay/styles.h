#pragma once

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

#define COLOR_BACKGROUND_REL LV_COLOR_MAKE(50,  50,  50)
#define COLOR_BACKGROUND_PR  LV_COLOR_MAKE(33,  34,  39)
#define COLOR_BORDER_REL     COLOR_BACKGROUND_REL
#define COLOR_BORDER_PR      LV_COLOR_MAKE(103, 214, 233)
#define COLOR_TXT_REL        LV_COLOR_MAKE(255, 255, 255)
#define COLOR_TXT_PR         LV_COLOR_MAKE(3,   255, 202)
#define COLOR_SCROLLBAR      LV_COLOR_MAKE(133, 133, 133)

#define BORDER_WIDTH 3

lv_style_t *get_bg_style();
lv_style_t *get_list_style();
lv_style_t *get_scrollbar_style();
lv_style_t *get_button_released_style();
lv_style_t *get_button_pressed_style();

void set_styles_font(lv_font_t *font);
void set_background_style(lv_obj_t *bg);
void set_list_styles(lv_obj_t *list);

#ifdef __cplusplus
}
#endif
