#include "overlay/styles.hpp"

namespace edz::ovl {

    static lv_style_t s_bgStyle = {
        .glass                      = 0,
        .body = { 
                  .main_color       = COLOR_BACKGROUND_REL,
                  .grad_color       = COLOR_BACKGROUND_REL,
                  .radius           = 0,
                  .opa              = LV_OPA_COVER,
                  .padding          = { 0, 0, 0, 0, 0 },
                }
    };

    static lv_style_t s_listStyle = {
        .glass                 = 0,
        .body = { .main_color            = COLOR_BACKGROUND_REL,
                  .grad_color            = COLOR_BACKGROUND_REL,
                  .radius                = 0,
                  .opa                   = LV_OPA_COVER,
                  .border = { .color     = COLOR_BORDER_REL,
                              .width     = 0,
                              .part      = LV_BORDER_NONE,
                              .opa       = LV_OPA_TRANSP
                  },
                  .shadow = { .color     = COLOR_BACKGROUND_REL,
                              .width     = 0,
                              .type      = LV_SHADOW_FULL
                  },
                  .padding               = { 0, 0, 1, 1, 4 }
        }
    };

    static lv_style_t s_scrollbarStyle = {
        .glass                      = 0,
        .body = { .main_color       = COLOR_SCROLLBAR,
                  .grad_color       = COLOR_SCROLLBAR,
                  .radius           = 2,
                  .opa              = LV_OPA_COVER,
                  .border  = { .width     = 0 },
                  .padding = { .inner    = LV_DPI / 20 }
        }
    };

    static lv_style_t s_btnPressedDisabledStyle = {
        .glass                           = 0,
        .body = { .main_color            = COLOR_BACKGROUND_PR,
                  .grad_color            = COLOR_BACKGROUND_PR,
                  .radius                = 0,
                  .opa                   = LV_OPA_COVER,
                  .border = { .color     = COLOR_BORDER_PR,
                              .width     = BORDER_WIDTH,
                              .part      = LV_BORDER_FULL,
                              .opa       = LV_OPA_COVER
                  },
                  .shadow = { .color     = COLOR_BACKGROUND_PR,
                              .width     = 0,
                              .type      = LV_SHADOW_FULL,
                  },
                  .padding               = { 0, 0, LV_DPI / 12, LV_DPI / 12, 0 },
        },     
        .text = { .color                 = COLOR_TXT_DIS_PR,
                  .letter_space          = 1,
                  .line_space            = 0,
                  .opa                   = LV_OPA_COVER
        }
    };

    static lv_style_t s_btnReleasedDisabledStyle = {
        .glass                           = 0,
        .body = { .main_color            = COLOR_BACKGROUND_REL,
                  .grad_color            = COLOR_BACKGROUND_REL,
                  .radius                = 0,
                  .opa                   = LV_OPA_COVER,
                  .border = { .color     = COLOR_BORDER_REL,
                              .width     = 1,
                              .part      = LV_BORDER_TOP,
                              .opa       = LV_OPA_COVER
                  },
                  .shadow = { .color     = COLOR_BACKGROUND_REL,
                              .width     = 0,
                              .type      = LV_SHADOW_FULL,
                  },
                  .padding               = { 0, 0, LV_DPI / 12, LV_DPI / 12, 0 },
        },
        .text = { .color                 = COLOR_TXT_DIS_REL,
                  .letter_space          = 1,
                  .line_space            = 0,
                  .opa                   = LV_OPA_COVER,
        }
    };

    static lv_style_t s_btnPressedEnabledStyle = {
        .glass                           = 0,
        .body = { .main_color            = COLOR_BACKGROUND_PR,
                  .grad_color            = COLOR_BACKGROUND_PR,
                  .radius                = 0,
                  .opa                   = LV_OPA_COVER,
                  .border = { .color     = COLOR_BORDER_PR,
                              .width     = BORDER_WIDTH,
                              .part      = LV_BORDER_FULL,
                              .opa       = LV_OPA_COVER
                  },
                  .shadow = { .color     = COLOR_BACKGROUND_PR,
                              .width     = 0,
                              .type      = LV_SHADOW_FULL,
                  },
                  .padding               = { 0, 0, LV_DPI / 12, LV_DPI / 12, 0 },
        },     
        .text = { .color                 = COLOR_TXT_EN_PR,
                  .letter_space          = 1,
                  .line_space            = 0,
                  .opa                   = LV_OPA_COVER
        }
    };

    static lv_style_t s_btnReleasedEnabledStyle = {
        .glass                           = 0,
        .body = { .main_color            = COLOR_BACKGROUND_REL,
                  .grad_color            = COLOR_BACKGROUND_REL,
                  .radius                = 0,
                  .opa                   = LV_OPA_COVER,
                  .border = { .color     = COLOR_BORDER_REL,
                              .width     = 1,
                              .part      = LV_BORDER_TOP,
                              .opa       = LV_OPA_COVER
                  },
                  .shadow = { .color     = COLOR_BACKGROUND_REL,
                              .width     = 0,
                              .type      = LV_SHADOW_FULL,
                  },
                  .padding               = { 0, 0, LV_DPI / 12, LV_DPI / 12, 0 },
        },
        .text = { .color                 = COLOR_TXT_EN_REL,
                  .letter_space          = 1,
                  .line_space            = 0,
                  .opa                   = LV_OPA_COVER
        }
    };

    static lv_style_t s_labelStyle = {
        .glass                           = 0,
        .text = { .color                 = LV_COLOR_WHITE,
                  .letter_space          = 1,
                  .line_space            = 0,
                  .opa                   = LV_OPA_COVER
        }
    };

    static lv_style_t s_labelTitleStyle = {
        .glass                           = 0,
        .text = { .color                 = LV_COLOR_WHITE,
                  .letter_space          = 1,
                  .line_space            = 0,
                  .opa                   = LV_OPA_COVER
        }
    };

    static lv_style_t s_lineStyle = {
        .glass                           = 0,
        .line = { .color            = LV_COLOR_WHITE,
                    .width              = 1,
                    .opa = 0xFF
        }
    };

    void setListStyle(lv_obj_t *list) {
        s_btnPressedEnabledStyle.text.font = LV_FONT_DEFAULT;
        s_btnReleasedEnabledStyle.text.font = LV_FONT_DEFAULT;
        s_btnPressedDisabledStyle.text.font = LV_FONT_DEFAULT;
        s_btnReleasedDisabledStyle.text.font = LV_FONT_DEFAULT;

        lv_obj_set_style(lv_scr_act(), &s_bgStyle);

        lv_list_set_style(list, LV_LIST_STYLE_BG,   &s_bgStyle);
        lv_list_set_style(list, LV_LIST_STYLE_SCRL, &s_listStyle);
        lv_list_set_style(list, LV_LIST_STYLE_SB, &s_scrollbarStyle);
        lv_list_set_style(list, LV_LIST_STYLE_BTN_TGL_PR, &s_btnPressedEnabledStyle);
        lv_list_set_style(list, LV_LIST_STYLE_BTN_TGL_REL, &s_btnReleasedEnabledStyle);
        lv_list_set_style(list, LV_LIST_STYLE_BTN_PR, &s_btnPressedDisabledStyle);
        lv_list_set_style(list, LV_LIST_STYLE_BTN_REL, &s_btnReleasedDisabledStyle);
    }

    void setLabelStyle(lv_obj_t *label) {
        s_labelStyle.text.font = LV_FONT_DEFAULT;
        lv_list_set_style(label, LV_LABEL_STYLE_MAIN, &s_labelStyle);
    }

    void setLabelTitleStyle(lv_obj_t *label) {
        s_labelTitleStyle.text.font = &lv_font_roboto_22;
        lv_list_set_style(label, LV_LABEL_STYLE_MAIN, &s_labelTitleStyle);
    }

    void setLineStyle(lv_obj_t *line) {
        lv_line_set_style(line, LV_LINE_STYLE_MAIN, &s_lineStyle);
    }

}