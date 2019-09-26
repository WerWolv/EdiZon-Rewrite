#include <lvgl.h>

#include "overlay/styles.h"

static lv_font_t *g_font = LV_FONT_DEFAULT;

static lv_style_t bg_style = {
    .glass                 = 0,
    .body.opa              = LV_OPA_COVER,
    .body.main_color       = COLOR_BACKGROUND_REL,
    .body.grad_color       = COLOR_BACKGROUND_REL,
    .body.padding          = { 0, 0, 0, 0, 0 },
    .body.radius           = 0,
    .body.padding.inner    = 0,
};

static lv_style_t list_style = {
    .glass                 = 0,
    .body.opa              = LV_OPA_COVER,
    .body.main_color       = COLOR_BACKGROUND_REL,
    .body.grad_color       = COLOR_BACKGROUND_REL,
    .body.radius           = 0,
    .body.padding          = { 1, 1, 1, 1, 4 },
    .body.border.color     = COLOR_BORDER_REL,
    .body.border.opa       = LV_OPA_TRANSP,
    .body.border.width     = 0,
    .body.border.part      = LV_BORDER_NONE,
    .body.shadow.color     = COLOR_BACKGROUND_REL,
    .body.shadow.type      = LV_SHADOW_FULL,
    .body.shadow.width     = 0,
};

static lv_style_t scrollbar_style = {
    .glass                 = 0,
    .body.opa              = LV_OPA_COVER,
    .body.main_color       = COLOR_SCROLLBAR,
    .body.grad_color       = COLOR_SCROLLBAR,
    .body.border.width     = 0,
    .body.radius           = 2,
    .body.padding.inner    = LV_DPI / 20,
};

static lv_style_t button_released_style_en = {
    .glass                 = 0,
    .body.opa              = LV_OPA_COVER,
    .body.main_color       = COLOR_BACKGROUND_REL,
    .body.grad_color       = COLOR_BACKGROUND_REL,
    .body.radius           = 0,
    .body.padding          = { LV_DPI / 10, LV_DPI / 10, LV_DPI / 12, LV_DPI / 12, 0 },
    .body.border.color     = COLOR_BORDER_REL,
    .body.border.opa       = LV_OPA_TRANSP,
    .body.border.width     = 0,
    .body.border.part      = LV_BORDER_NONE,
    .body.shadow.color     = COLOR_BACKGROUND_REL,
    .body.shadow.type      = LV_SHADOW_FULL,
    .body.shadow.width     = 0,
    .text.opa              = LV_OPA_COVER,
    .text.color            = LV_COLOR_MAKE(50, 255, 50),
    .text.letter_space     = 1,
    .text.line_space       = 0,
};

static lv_style_t button_released_style_dis = {
    .glass                 = 0,
    .body.opa              = LV_OPA_COVER,
    .body.main_color       = COLOR_BACKGROUND_REL,
    .body.grad_color       = COLOR_BACKGROUND_REL,
    .body.radius           = 0,
    .body.padding          = { LV_DPI / 10, LV_DPI / 10, LV_DPI / 12, LV_DPI / 12, 0 },
    .body.border.color     = COLOR_BORDER_REL,
    .body.border.opa       = LV_OPA_TRANSP,
    .body.border.width     = 0,
    .body.border.part      = LV_BORDER_NONE,
    .body.shadow.color     = COLOR_BACKGROUND_REL,
    .body.shadow.type      = LV_SHADOW_FULL,
    .body.shadow.width     = 0,
    .text.opa              = LV_OPA_COVER,
    .text.color            = COLOR_TXT_REL,
    .text.letter_space     = 1,
    .text.line_space       = 0,
};

static lv_style_t button_pressed_style = {
    .glass                 = 0,
    .body.opa              = LV_OPA_COVER,
    .body.main_color       = COLOR_BACKGROUND_PR,
    .body.grad_color       = COLOR_BACKGROUND_PR,
    .body.radius           = BORDER_WIDTH,
    .body.padding          = { LV_DPI / 10, LV_DPI / 10, LV_DPI / 12, LV_DPI / 12, 0 },
    .body.border.color     = COLOR_BORDER_PR,
    .body.border.opa       = LV_OPA_COVER,
    .body.border.width     = BORDER_WIDTH,
    .body.border.part      = LV_BORDER_FULL,
    .body.shadow.color     = COLOR_BACKGROUND_PR,
    .body.shadow.type      = LV_SHADOW_FULL,
    .body.shadow.width     = 0,
    .text.opa              = LV_OPA_COVER,
    .text.color            = COLOR_TXT_PR,
    .text.letter_space     = 1,
    .text.line_space       = 0,
};

lv_style_t *get_bg_style() {
    return &bg_style;
}

lv_style_t *get_list_style() {
    return &list_style;
}

lv_style_t *get_scrollbar_style() {
    return &scrollbar_style;
}

lv_style_t *get_button_released_style_enabled() {
    return &button_released_style_en;
}

lv_style_t *get_button_released_style_disabled() {
    return &button_released_style_dis;
}

lv_style_t *get_button_pressed_style() {
    return &button_pressed_style;
}

void set_styles_font(lv_font_t *font) {
    g_font = font;
}

void set_background_style(lv_obj_t *bg) {
    lv_obj_set_style(bg, &bg_style);
}

void set_list_styles(lv_obj_t *list) {
    get_button_released_style_enabled()->text.font = g_font;
    get_button_released_style_disabled()->text.font = g_font;
    get_button_pressed_style()->text.font  = g_font;
    lv_list_set_style(list, LV_LIST_STYLE_BG,   &list_style);
    lv_list_set_style(list, LV_LIST_STYLE_SCRL, &list_style);
    lv_list_set_style(list, LV_LIST_STYLE_SB, &scrollbar_style);
    lv_list_set_style(list, LV_LIST_STYLE_BTN_REL, &button_released_style_dis);
    lv_list_set_style(list, LV_LIST_STYLE_BTN_PR, &button_pressed_style);
}