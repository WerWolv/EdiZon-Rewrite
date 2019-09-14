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

#include <switch.h>

#include "overlay/font.hpp"

#define STBTT_RASTERIZER_VERSION 1
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

namespace edz::ovl {

    EResult BuiltinFont::initialize() {
        EResult res = 0;

        if (EResult(res = plInitialize()).failed())
            goto end;

        if (EResult(res = plGetSharedFontByType(&BuiltinFont::g_stdFont, PlSharedFontType_Standard)).failed())
            goto close_serv;

        if (EResult(res = plGetSharedFontByType(&BuiltinFont::g_extFont, PlSharedFontType_NintendoExt)).failed())
            goto close_serv;

        goto end;

    close_serv:
        plExit();
    end:
        return res;
    }

    void BuiltinFont::exit() {
        plExit();
    }

    PlFontData *BuiltinFont::getFont(u32 chr) const {
        if ((0x20 <= chr) && (chr <= 0x266b))
            return &BuiltinFont::g_stdFont;
        else if ((0xe000 <= chr) && (chr <= 0xe153))
            return &BuiltinFont::g_extFont;
        return nullptr;
    }

    EResult FreetypeFont::initialize() {
        EResult res = 0;
        FT_Error ft_rc = 0;

        if (EResult(res = BuiltinFont::initialize()).failed())
            goto finalize;

        if (EResult(ft_rc = FT_Init_FreeType(&FreetypeFont::g_ftLib)).failed())
            goto finalize;

        if (EResult(ft_rc = FT_New_Memory_Face(FreetypeFont::g_ftLib, (FT_Bytes)BuiltinFont::g_stdFont.address, BuiltinFont::g_stdFont.size, 0, &FreetypeFont::g_stdFace)).failed())
            goto close_ft;

        if (EResult(ft_rc = FT_New_Memory_Face(FreetypeFont::g_ftLib, (FT_Bytes)BuiltinFont::g_extFont.address, BuiltinFont::g_extFont.size, 0, &FreetypeFont::g_extFace)).failed())
            goto close_std_face;

        goto end;

    close_std_face:
        FT_Done_Face(FreetypeFont::g_stdFace);
    close_ft:
        FT_Done_FreeType(FreetypeFont::g_ftLib);
    finalize:
        exit();
    end:

        return (EResult)(res | ft_rc);
    }

    void FreetypeFont::exit() {
        FT_Done_Face(FreetypeFont::g_extFace);
        FT_Done_Face(FreetypeFont::g_stdFace);
        FT_Done_FreeType(FreetypeFont::g_ftLib);
        BuiltinFont::exit();
    }

    FreetypeFont::FreetypeFont() {

        setFontSize(0, DEFAULT_FONT_HEIGHT);
        loadStdFace();
    }

    FreetypeFont::~FreetypeFont() {

    }

    FT_Face FreetypeFont::getFace(u32 chr) {
        PlFontData *font = getFont(chr);
        if (font == &BuiltinFont::g_stdFont)
            FreetypeFont::g_lastFace = FreetypeFont::g_stdFace;
        else if (font == &BuiltinFont::g_extFont)
            FreetypeFont::g_lastFace = FreetypeFont::g_extFace;
        else
            FreetypeFont::g_lastFace = nullptr;
        return getLastFace();
    }

    void FreetypeFont::setFontSize(FT_UInt width, FT_UInt height) {
        FT_Set_Pixel_Sizes(FreetypeFont::g_stdFace, width, height);
        FT_Set_Pixel_Sizes(FreetypeFont::g_extFace, width, height);
    }

    bbox FreetypeFont::getFontBbox() const {
        FT_BBox b = getLastFace()->bbox;
        return { (float)b.xMin, (float)b.yMin, (float)b.xMax, (float)b.yMax };
    }

    void FreetypeFont::loadGlyph(u32 chr, u32 flags) {
        FT_Face face = getFace(chr);
        if (face == nullptr)
            return;
        
        FT_UInt glyphIndex = FT_Get_Char_Index(face, chr);
        if (glyphIndex == 0) return;

        if (!FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT))
            FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO);

    }

    EResult StbFont::initialize() {
        EResult res = 0;

        if (EResult(res = BuiltinFont::initialize()).failed())
            goto finalize;

        if (stbtt_InitFont(&StbFont::g_stdInfo, (u8 *)StbFont::g_stdFont.address, 0) == 0) {
            res = 0xabcd;
            goto finalize;
        }

        if (stbtt_InitFont(&StbFont::g_extInfo, (u8 *)StbFont::g_extFont.address, 0) == 0) {
            res = 0xabcde;
            goto finalize;
        }

        goto end;

    finalize:
        exit();
    end:
        return res;
    }

    void StbFont::exit() {
        BuiltinFont::exit();
    }

    StbFont::StbFont() {
        setFontSize(0, DEFAULT_FONT_HEIGHT);
    }

    StbFont::~StbFont() {
        free(this->m_bitmap);
    }

    void StbFont::setFontSize(u32 width, u32 height) {
        IGNORE_ARG(width);
        this->m_scale = stbtt_ScaleForPixelHeight(&StbFont::g_stdInfo, height);
        this->m_bitmap = realloc(this->m_bitmap, getFontMaxWidth() * getFontMaxHeight());
        
    }

    stbtt_fontinfo *StbFont::getFontInfo(u32 chr) {
        PlFontData *font = getFont(chr);
        if (font == &StbFont::g_stdFont)
            this->m_lastInfo = &StbFont::g_stdInfo;
        else if (font == &StbFont::g_extFont)
            this->m_lastInfo = &StbFont::g_extInfo;
        else
            this->m_lastInfo = NULL;
        return getLastInfo();
    }

    bbox StbFont::getFontBbox() const {
        int x_min, y_min, x_max, y_max;
        stbtt_GetFontBoundingBox(&this->StbFont::g_stdInfo, &x_min, &y_min, &x_max, &y_max);
        return { (float)x_min, (float)y_min, (float)x_max, (float)y_max };
    }

    void StbFont::loadGlyph(u32 chr, u32 flags) {
        IGNORE_ARG(flags);
        this->m_lastInfo = getFontInfo(chr);
        this->m_lastGlyph = stbtt_FindGlyphIndex(getLastInfo(), chr);
    }

    void StbFont::loadGlyphRender(u32 chr) {
        loadGlyph(chr);
        memset(this->m_bitmap, 0, getFontMaxWidth() * getFontMaxHeight());
        stbtt_MakeGlyphBitmap(getLastInfo(), (u8 *)this->m_bitmap, getCharWidth(chr), getCharHeight(chr), getCharWidth(),
            getScale(), getScale(), getLastGlyph());
    }

    const void *LvglFont::getCharRawBuf() {
        unsigned int off = getCharWidth() * (getFontAscender() - getCharBitmapTop()), sz = getCharWidth() * getCharHeight();
        u8 tmp[sz];
        memcpy(tmp, StbFont::getCharRawBuf(), sz);
        memcpy((u8 *)this->m_bitmap + off, tmp, sz);
        memset(this->m_bitmap, 0, off);
        
        return this->m_bitmap;
    }

}

