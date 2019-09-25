#include "overlay/gui.hpp"

namespace edz::ovl {

    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;

    Gui::Gui(Screen *screen) {
        Gui::s_screen = screen;

        last_x = 0;
        last_y = 0;

        static lv_color_t buf[LV_HOR_RES_MAX * LV_VER_RES_MAX][2];

        lv_disp_drv_init(&this->m_displayDriver);
        lv_disp_buf_init(&this->m_displayBuffer, buf[0], buf[1], LV_HOR_RES_MAX * LV_VER_RES_MAX);
        this->m_displayDriver.flush_cb = Gui::lvglDisplayFlush;
        this->m_displayDriver.buffer = &this->m_displayBuffer;
        this->m_displayDriver.hor_res = LV_HOR_RES_MAX;
        this->m_displayDriver.ver_res = LV_VER_RES_MAX;
        lv_disp_drv_register(&this->m_displayDriver);

        lv_indev_drv_init(&this->m_inputDevice);
        this->m_inputDevice.type = LV_INDEV_TYPE_POINTER;
        this->m_inputDevice.read_cb = Gui::lvglTouchRead;
        lv_indev_drv_register(&this->m_inputDevice);
    }

    Gui::~Gui() {

    }

    bool Gui::shouldClose() {
        return last_x > 256;
    }


    void Gui::lvglDisplayFlush(lv_disp_drv_t *displayDriver, const lv_area_t *area, lv_color_t *color) {
        
        for(u16 y = area->y1; y <= area->y2; y++) {
            for(u16 x = area->x1; x <= area->x2; x++) {
                Gui::s_screen->setPixel(x, y, edz::ovl::makeColor<edz::ovl::rgba4444_t>(color->ch.red >> 4, color->ch.green >> 4, color->ch.blue >> 4));
                color++;
            }
        }
        
        Gui::s_screen->flush();

        lv_disp_flush_ready(displayDriver);
    }

    bool Gui::lvglTouchRead(_lv_indev_drv_t * indev, lv_indev_data_t * data) {
        /*Save the state and save the pressed coordinate*/
        data->state = (hidKeysHeld(CONTROLLER_P1_AUTO) & KEY_TOUCH) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL; 
        if(data->state == LV_INDEV_STATE_PR) {
            touchPosition pos;
            hidTouchRead(&pos, 0);
            last_x = pos.px;
            last_y = ((pos.py) / 720.0) * 512;
        }
    
        /*Set the coordinates (if released use the last pressed coordinates)*/
        data->point.x = last_x;
        data->point.y = last_y;

        return false; /*Return `false` because we are not buffering and no more data to read*/
    }

}