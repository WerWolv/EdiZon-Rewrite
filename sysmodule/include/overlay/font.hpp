#pragma once

#include <switch.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#define STBTT_RASTERIZER_VERSION 1
#include <stb_truetype.h>

#include "overlay/constants.hpp"

struct bbox { float xMin, yMin, xMax, yMax; };

class BuiltinFont {
    protected:
        static inline PlFontData g_stdFont, g_extFont;

    public:
        static Result initialize();
        static void finalize();

        BuiltinFont() = default;
        virtual ~BuiltinFont() = default;

        inline PlFontData *getFont(PlSharedFontType type) {
            switch (type) {
                case PlSharedFontType_Standard:
                    return &BuiltinFont::g_stdFont;
                case PlSharedFontType_NintendoExt:
                    return &BuiltinFont::g_extFont;
                default:
                    return nullptr;
            }
        }

        PlFontData *getFont(u32 chr) const;
};

class FreetypeFont : public BuiltinFont {
    protected:
        static inline FT_Library g_ftLib;
        static inline FT_Face g_stdFace, g_extFace, g_lastFace;

    public:
        static Result initialize();
        static void finalize();

        FreetypeFont();
        virtual ~FreetypeFont();

        FT_Face getFace(u32 chr);
        inline FT_Face getStdFace()  const { return FreetypeFont::g_stdFace; }
        inline FT_Face getExtFace()  const { return FreetypeFont::g_extFace; }
        inline FT_Face getLastFace() const { return FreetypeFont::g_lastFace; };

        inline void loadStdFace() const { FreetypeFont::g_lastFace = FreetypeFont::g_stdFace; };
        inline void loadExtFace() const { FreetypeFont::g_lastFace = FreetypeFont::g_extFace; };

        virtual void setFontSize(FT_UInt width, FT_UInt height);

        bbox getFontBbox() const;
        s32 getFontMaxWidth()  const { return ((s32)(getFontBbox().xMax - getFontBbox().xMin)) >> 6; }
        s32 getFontMaxHeight() const { return ((s32)(getFontBbox().yMax - getFontBbox().yMin)) >> 6; }
        s32 getFontAscender()  const { return getLastFace()->ascender >> 6; }
        s32 getFontDescender() const { return getLastFace()->descender >> 6; }

        u32 getCharIndex(u32 chr) { return FT_Get_Char_Index(getFace(chr), chr); }

        void loadGlyph(u32 chr, u32 flags);
        void loadGlyphMetrics(u32 chr) { return loadGlyph(chr, FT_LOAD_BITMAP_METRICS_ONLY); }
        void loadGlyphRender(u32 chr)  { return loadGlyph(chr, FT_LOAD_RENDER); }

        s32 getCharStride()      const { return getLastFace()->glyph->bitmap.pitch; }
        s32 getCharStride(u32 chr)     { loadGlyphMetrics(chr); return getCharStride(); }
        s32 getCharWidth()       const { return getLastFace()->glyph->bitmap.width; }
        s32 getCharWidth(u32 chr)      { loadGlyphMetrics(chr); return getCharWidth(); }
        s32 getCharHeight()      const { return getLastFace()->glyph->bitmap.rows; }
        s32 getCharHeight(u32 chr)     { loadGlyphMetrics(chr); return getCharHeight(); }
        s32 getCharBitmapLeft()  const { return getLastFace()->glyph->bitmap_left; }
        s32 getCharBitmapLeft(u32 chr) { loadGlyphMetrics(chr); return getCharBitmapLeft(); }
        s32 getCharBitmapTop()   const { return getLastFace()->glyph->bitmap_top; }
        s32 getCharBitmapTop(u32 chr)  { loadGlyphMetrics(chr); return getCharBitmapLeft(); }

        FT_GlyphSlot  getGlyph()             const { return getLastFace()->glyph; }
        FT_GlyphSlot  getGlyph(u32 chr)            { loadGlyphRender(chr); return getLastFace()->glyph; }
        FT_Bitmap     getCharBitmap()        const { return getGlyph()->bitmap; }
        FT_Bitmap     getCharBitmap(u32 chr)       { return getGlyph(chr)->bitmap; }
        virtual const void *getCharRawBuf()  const { return getCharBitmap().buffer; }
        virtual const void *getCharRawBuf(u32 chr) { return getCharBitmap(chr).buffer; }
};

class StbFont : public BuiltinFont {
    protected:
        static inline stbtt_fontinfo g_stdInfo, g_extInfo;
        stbtt_fontinfo *m_lastInfo;
        float m_scale;
        int m_lastGlyph;
        void *m_bitmap = nullptr;

    private:
        inline const float getScale() const { return this->m_scale; }
        inline const int getLastGlyph() const { return this->m_lastGlyph; }

    public:
        static Result initialize();
        static void finalize();

        StbFont();
        virtual ~StbFont();

        stbtt_fontinfo *getFontInfo(u32 chr);
        inline stbtt_fontinfo *getStdInfo()  const { return &StbFont::g_stdInfo; }
        inline stbtt_fontinfo *getExtInfo()  const { return &StbFont::g_extInfo; }
        inline stbtt_fontinfo *getLastInfo() const { return this->m_lastInfo; }

        void setFontSize(u32 width, u32 height);

        bbox getFontBbox() const;
        inline s32 getFontMaxWidth()  const { bbox b = getFontBbox(); return getScale() * (b.xMax - b.xMin) + 1; }
        inline s32 getFontMaxHeight() const { bbox b = getFontBbox(); return getScale() * (b.yMax - b.yMin) + 1; }
        inline s32 getFontAscender()  const { int a; stbtt_GetFontVMetrics(getLastInfo(), &a, NULL, NULL); return getScale() * a + 1; }
        inline s32 getFontDescender() const { int d; stbtt_GetFontVMetrics(getLastInfo(), NULL, &d, NULL); return getScale() * d + 1; }

        inline u32 getCharIndex(u32 chr) { this->m_lastGlyph = stbtt_FindGlyphIndex(getFontInfo(chr), chr); return this->m_lastGlyph; }

        void loadGlyph(u32 chr, u32 flags=0);
        void loadGlyphMetrics(u32 chr) { loadGlyphRender(chr); }
        void loadGlyphRender(u32 chr);

        inline s32 getCharStride() const
            { int s; stbtt_GetGlyphHMetrics(getLastInfo(), getLastGlyph(), &s, NULL); return getScale() * s + 1; }
        inline s32 getCharStride(u32 chr)     { loadGlyph(chr); return getCharStride(); }
        inline s32 getCharWidth() const
            { int x0, x1; stbtt_GetGlyphBox(getLastInfo(), getLastGlyph(), &x0, NULL, &x1, NULL); return getScale() * (x1 - x0) + 1; }
        inline s32 getCharWidth(u32 chr)      { loadGlyph(chr); return getCharWidth(); }
        inline s32 getCharHeight() const
            { int y0, y1; stbtt_GetGlyphBox(getLastInfo(), getLastGlyph(), NULL, &y0, NULL, &y1); return getScale() * (y1 - y0) + 1; }
        inline s32 getCharHeight(u32 chr)     { loadGlyph(chr); return getCharHeight(); }
        inline s32 getCharAdvance(u32 chr) { s32 advance = 0; stbtt_GetCodepointHMetrics(getFontInfo(chr), chr, &advance, NULL); return advance * getScale(); }
        inline s32 getCharBitmapLeft() const
            { int l; stbtt_GetGlyphBitmapBox(getLastInfo(), getLastGlyph(), getScale(), getScale(), &l, NULL, NULL, NULL); return abs(l); }
        inline s32 getCharBitmapLeft(u32 chr) { loadGlyphRender(chr); return getCharBitmapLeft(); }
        inline s32 getCharBitmapTop() const
            { int t; stbtt_GetGlyphBitmapBox(getLastInfo(), getLastGlyph(), getScale(), getScale(), NULL, &t, NULL, NULL); return abs(t); }
        inline s32 getCharBitmapTop(u32 chr)  { loadGlyphRender(chr); return getCharBitmapTop(); }

        virtual inline const void *getCharRawBuf()  const { return this->m_bitmap; }
        virtual inline const void *getCharRawBuf(u32 chr) { loadGlyphRender(chr); return getCharRawBuf(); }
};
