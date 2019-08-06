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

#pragma once

#include <switch.h>

#include <vector>
#include <string>
#include <unordered_map>

#include "overlay/constants.hpp"
#include "overlay/color.hpp"
#include "overlay/font.hpp"

#include "helpers/result.hpp"

namespace edz::ovl {

    enum class TextAlignment {
        ALIGNED_LEFT,
        ALIGNED_CENTER,
        ALIGNED_RIGHT
    };

    class Screen {
        private:
            static inline ViDisplay g_display;
            static inline Event g_vsyncEvent;

            ViLayer m_layer;
            NWindow m_window;
            Framebuffer m_frameBufferObject;

            void *m_frameBuffer = nullptr;
            std::unordered_map<size_t, std::pair<u16, u16>> m_stringDimensions;

        public:
            static EResult initialize();
            static void finalize();

            static inline EResult waitForVsync() { return eventWait(&g_vsyncEvent, U64_MAX); }

            Screen();
            ~Screen();

            const inline u32 getWindowWidth()       const { return this->m_window.width; }
            const inline u32 getWindowHeight()      const { return this->m_window.height; }
            const inline u32 getWindowFormat()      const { return this->m_window.format; }
            const inline u32 getFramebufferWidth()  const { return this->m_frameBufferObject.width_aligned; }
            const inline u32 getFramebufferHeight() const { return this->m_frameBufferObject.height_aligned; }
            const inline u32 getFramebufferSize()   const { return this->m_frameBufferObject.fb_size; }
            const inline u32 getFramebufferStride() const { return this->m_frameBufferObject.stride; }

            inline void *getFramebufferAddress(u32 slot) const { return (u8 *)this->m_frameBufferObject.buf + slot * this->m_frameBufferObject.fb_size; }
            inline u32 getCurFramebufferSlot()           const { return this->m_window.cur_slot; }
            inline u32 getNextFramebufferSlot()          const { return (getCurFramebufferSlot() + 1) % this->m_frameBufferObject.num_fbs; }

            void *getCurFramebuffer();
            inline void *getNextFramebuffer() const { return getFramebufferAddress(getNextFramebufferSlot()); }

            void flush();

            inline const u16 getPixelRaw(u32 off)        { return ((u16 *)getCurFramebuffer())[off]; }
            inline const u16 getPixelRaw(u32 x, u32 y)   { return getPixelRaw(getPixelOffset(x, y)); }
            inline const rgba4444_t getPixel(u32 off)      { return makeColor<rgba4444_t>(((u16 *)getCurFramebuffer())[off]); }
            inline const rgba4444_t getPixel(u32 x, u32 y) { return getPixel(getPixelOffset(x, y)); }

            inline u8 blendColor(u32 src, u32 dst, u8 alpha) {
                u8 one_minus_alpha = static_cast<u8>(0xF) - alpha;
                return (dst*alpha + src*one_minus_alpha) / static_cast<u8>(0xF);
            }

            inline void setPixelBlend(u32 off, u16 color) {
                rgba4444_t src(((u16 *)getCurFramebuffer())[off]);
                rgba4444_t dst(color);
                rgba4444_t end(0);

                end.r = blendColor(src.r, dst.r, dst.a);
                end.g = blendColor(src.g, dst.g, dst.a);
                end.b = blendColor(src.b, dst.b, dst.a);
                end.a = 0xF;

                ((u16 *)getCurFramebuffer())[off] = end.rgba;
            }

            inline void setPixelBlend(u32 x, u32 y, u16 color)                  { setPixelBlend(getPixelOffset(x, y), color); }
            inline void setPixelBlend(u32 off, rgba4444_t color)                { setPixelBlend(off, color.rgba); }
            inline void setPixelBlend(u32 x, u32 y, rgba4444_t color)           { setPixelBlend(getPixelOffset(x, y), color); }
            inline void setPixelBlend(u32 off, rgba4444_t color, u8 alpha)      { setPixelBlend(off, blend(color, getPixel(off), alpha).rgba); }
            inline void setPixelBlend(u32 x, u32 y, rgba4444_t color, u8 alpha) { setPixelBlend(getPixelOffset(x, y), color, alpha); }
            
            inline void setPixel(u32 off, u16 color)                       { ((u16 *)getCurFramebuffer())[off] = color; }
            inline void setPixel(u32 x, u32 y, u16 color)                  { setPixel(getPixelOffset(x, y), color); }
            inline void setPixel(u32 off, rgba4444_t color)                { setPixel(off, color.rgba); }
            inline void setPixel(u32 x, u32 y, rgba4444_t color)           { setPixel(getPixelOffset(x, y), color); }
            inline void setPixel(u32 off, rgba4444_t color, u8 alpha)      { setPixel(off, blend(color, getPixel(off), alpha).rgba); }
            inline void setPixel(u32 x, u32 y, rgba4444_t color, u8 alpha) { setPixel(getPixelOffset(x, y), color, alpha); }

            void clear();
            void fill(u16 color);
            void fill(rgba4444_t color);
            void fillArea(s32 x1, s32 y1, s32 x2, s32 y2, u16 color);
            void fillArea(s32 x1, s32 y1, s32 x2, s32 y2, rgba4444_t color);
            void mapArea(s32 x1, s32 y1, s32 x2, s32 y2, u16 *area);
            void mapArea(s32 x1, s32 y1, s32 x2, s32 y2, u8 *area);
            void mapArea(s32 x1, s32 y1, s32 x2, s32 y2, rgba4444_t *area);

            void drawGlyph(s16 x, s16 y, rgba4444_t clr, u32 character, StbFont *font);

            void drawText_(StbFont *font, s16 x, s16 y, rgba4444_t clr, const char* text, s32 max_width, const char* end_text);
            void drawText(StbFont *font, s16 x, s16 y, rgba4444_t clr, const char* text);
            void drawTextAligned(StbFont *font, s16 x, s16 y, rgba4444_t clr, const char* text, TextAlignment alignment);
            void drawTextTruncate(StbFont *font, s16 x, s16 y, rgba4444_t clr, const char* text, u32 max_width, const char* end_text);
            void getTextDimensions(StbFont *font, const char* text, u32* width_out, u32* height_out);

            inline const u32 getPixelOffset(u32 x, u32 y) const {
                // Swizzling pattern:
                //    y8,y7,y6,y5,y4,y3,y2,y1,y0,x7,x6,x5,x4,x3,x2,x1,x0
                // -> y8,y7,x7,x6,x5,y6,y5,y4,y3,x4,y2,y1,x3,y0,x2,x1,x0
                const u32 swizzled_x = ((x & 0b11100000) * 128) + ((x & 0b00010000) * 8) + ((x & 0b00001000) * 2) + (x & 0b00000111);
                const u32 swizzled_y = ((y & 0b110000000) * 256) + ((y & 0b001111000) * 32) + ((y & 0b000000110) * 16) + ((y & 0b000000001) * 8);
                return swizzled_x + swizzled_y;
            }
    };

}

