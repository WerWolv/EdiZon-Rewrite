/**
 * Copyright (C) 2019 averne
 * Copyright (C) 2019 WerWolv
 * 
 * This file is part of EdiZon.
 * 
 * EdiZon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 * 
 * EdiZon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with EdiZon.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstdlib>
#include <switch.h>
#include <cstring>
#include <algorithm>

#include "overlay/constants.hpp"
#include "overlay/color.hpp"
#include "helpers/vi_shim.hpp"

#include "overlay/screen.hpp"

extern "C" u64 __nx_vi_layer_id;

namespace edz::ovl {

    EResult Screen::initialize() {
        EResult res = 0;

        if (EResult(res = viInitialize(ViServiceType_Manager)).failed())
            goto end;

        if (EResult(res = viOpenDefaultDisplay(&Screen::g_display)).failed())
            goto close_serv;

        if (EResult(res = viGetDisplayVsyncEvent(&Screen::g_display, &Screen::g_vsyncEvent)).failed())
            goto close_display;

        goto end;

    close_display:
        viCloseDisplay(&Screen::g_display);
    close_serv:
        viExit();
    end:

        return res;
    }

    void Screen::exit() {

        eventClose(&Screen::g_vsyncEvent);
        viCloseDisplay(&Screen::g_display);
        viExit();
    }

    Screen::Screen() {

        EResult res = 0;
        u64 layer_id = 0;

        if (EResult(res = viCreateManagedLayer(&Screen::g_display, (ViLayerFlags)0, 0, &__nx_vi_layer_id)).failed())
            goto fatal;

        if (EResult(res = viCreateLayer(&Screen::g_display, &this->m_layer)).failed())
            goto close_managed_layer;

        if (EResult(res = viSetLayerScalingMode(&this->m_layer, ViScalingMode_PreserveAspectRatio)).failed())
            goto close_layer;

        s32 layer_z;
        if (EResult(res = viGetZOrderCountMax(&Screen::g_display, &layer_z)).succeeded() && (layer_z > 0)) {
            if (EResult(res = viSetLayerZ(&this->m_layer, layer_z)).failed())
                goto close_layer;
        }

        if (EResult(res = viSetLayerSize(&this->m_layer, LAYER_WIDTH, LAYER_HEIGHT)).failed())
            goto close_layer;

        if (EResult(res = viSetLayerPosition(&this->m_layer, LAYER_X, LAYER_Y)).failed())
            goto close_layer;

        if (EResult(res = nwindowCreateFromLayer(&this->m_window, &this->m_layer)).failed())
            goto close_layer;

        if (EResult(res = framebufferCreate(&this->m_frameBufferObject, &this->m_window, FB_WIDTH, FB_HEIGHT, PIXEL_FORMAT_RGBA_4444, 2)).failed())
            goto close_window;

        return;

    close_window:
        nwindowClose(&this->m_window);
    close_layer:
        layer_id = this->m_layer.layer_id;
        viCloseLayer(&this->m_layer);
    close_managed_layer:
        layer_id = (layer_id == 0) ? this->m_layer.layer_id : layer_id;
        viDestroyManagedLayer(layer_id);
    fatal:
        exit();

        fatalThrow(res);
    }

    Screen::~Screen() {

        flush();
        framebufferClose(&this->m_frameBufferObject);
        nwindowClose(&this->m_window);
        u64 layer_id = this->m_layer.layer_id; // viCloseLayer zeroes it out
        viCloseLayer(&this->m_layer);
        viDestroyManagedLayer(layer_id);
    }

    void *Screen::getCurFramebuffer() {
        if (!this->m_frameBuffer)
            this->m_frameBuffer = framebufferBegin(&this->m_frameBufferObject, NULL);
        return this->m_frameBuffer;
    }

    void Screen::flush() {
        // Make sure a buffer is dequeued, and report modifications on the second buffer
        std::memcpy(getNextFramebuffer(), getCurFramebuffer(), getFramebufferSize());
        framebufferEnd(&this->m_frameBufferObject);
        this->m_frameBuffer = nullptr; // Wait for vsync before dequeing a buffer
    }

    void Screen::clear() {
        fill((u16)0);
    }

    void Screen::fill(u16 color) {
        std::fill((u16 *)getCurFramebuffer(), (u16 *)((u8 *)getCurFramebuffer()+getFramebufferSize()), color);
    }

    void Screen::fill(rgba4444_t color) {
        fill(color.rgba);
    }

    void Screen::fillArea(s32 x1, s32 y1, s32 x2, s32 y2, u16 color) {
        for (s32 y=y1; y<y2; ++y) {
            for (s32 x=x1; x<x2; ++x)
                setPixel(x, y, color);
        }
    }

    void Screen::fillArea(s32 x1, s32 y1, s32 x2, s32 y2, rgba4444_t color) {
        for (s32 y=y1; y<y2; ++y) {
            for (s32 x=x1; x<x2; ++x)
                setPixel(x, y, color.rgba);
        }
    }

    void Screen::mapArea(s32 x1, s32 y1, s32 x2, s32 y2, u16 *area) {
        for (s32 y=y1; y<y2; ++y) {
            for (s32 x=x1; x<x2; ++x)
                setPixel(x, y, *(area++));
        }
    }

    void Screen::mapArea(s32 x1, s32 y1, s32 x2, s32 y2, rgba4444_t *area) {
        for (s32 y=y1; y<y2; ++y) {
            for (s32 x=x1; x<x2; ++x)
                setPixel(x, y, (*(area++)).rgba);
        }
    }

    static inline u8 decodeByte(const char** ptr) {
        u8 c = static_cast<u8>(**ptr);
        (*ptr)++;

        return c;
    }

    // UTF-8 code adapted from http://www.json.org/JSON_checker/utf8_decode.c

    static inline s8 decodeUTF8Cont(const char** ptr) {
        s32 c = decodeByte(ptr);
        return ((c & 0xC0) == 0x80) ? (c & 0x3F) : -1;
    }

    static inline u32 decodeUTF8(const char** ptr) {
        u32 r;
        u8 c;
        s8 c1, c2, c3;

        c = decodeByte(ptr);

        if ((c & 0x80) == 0)
            return c;

        if ((c & 0xE0) == 0xC0) {
            c1 = decodeUTF8Cont(ptr);
            if (c1 >= 0) {
                r = ((c & 0x1F) << 6) | c1;
            if (r >= 0x80)
                return r;
            }
        } else if ((c & 0xF0) == 0xE0) {
            c1 = decodeUTF8Cont(ptr);
            if (c1 >= 0) {
                c2 = decodeUTF8Cont(ptr);
                if (c2 >= 0) {
                    r = ((c & 0x0F) << 12) | (c1 << 6) | c2;
                    if (r >= 0x800 && (r < 0xD800 || r >= 0xE000))
                        return r;
                }
            }
        } else if ((c & 0xF8) == 0xF0) {
            c1 = decodeUTF8Cont(ptr);
            if (c1 >= 0) {
                c2 = decodeUTF8Cont(ptr);
                if (c2 >= 0) {
                    c3 = decodeUTF8Cont(ptr);
                    if (c3 >= 0) {
                        r = ((c & 0x07) << 18) | (c1 << 12) | (c2 << 6) | c3;
                        if (r >= 0x10000 && r < 0x110000)
                            return r;
                    }
                }
            }
        }

        return 0xFFFD;
    }

}
