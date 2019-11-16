#pragma once

#include <lvgl.h>

namespace edz::ovl {
        
    #define COLOR_BACKGROUND_REL ((lv_color_t){{10, 10, 10, 0xD0}})
    #define COLOR_BACKGROUND_PR  ((lv_color_t){{40, 10, 10, 0xD0}})
    #define COLOR_TRANSPARENT    ((lv_color_t){{0, 0, 0, 0}})
    #define COLOR_BORDER_REL     ((lv_color_t){{0x80, 0x80, 0x80, 0xFF}})
    #define COLOR_BORDER_PR      LV_COLOR_MAKE(103, 214, 233)
    #define COLOR_TXT_DIS_REL    LV_COLOR_MAKE(255, 255, 255)
    #define COLOR_TXT_DIS_PR     LV_COLOR_MAKE(3,   255, 202)
    #define COLOR_TXT_EN_REL     LV_COLOR_MAKE(100, 255, 100)
    #define COLOR_TXT_EN_PR      LV_COLOR_MAKE(3,   255, 102)
    #define COLOR_SCROLLBAR      LV_COLOR_MAKE(133, 133, 133)

    #define BORDER_WIDTH 3

    void setBackgroundStyle();
    void setPageStyle(lv_obj_t *page);
    void setListStyle(lv_obj_t *list);
    void setLabelStyle(lv_obj_t *label);
    void setLabelTitleStyle(lv_obj_t *label);
    void setLineStyle(lv_obj_t *line);
    void setButtonStyle(lv_obj_t *button);

    void setGroupStyle(lv_group_t *group);
}