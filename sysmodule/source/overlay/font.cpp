#include <switch.h>

#include "overlay/font.hpp"

#define STBTT_RASTERIZER_VERSION 1
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

Result BuiltinFont::initialize() {
    Result rc = 0;

    if (R_FAILED(rc = plInitialize()))
        goto end;

    if (R_FAILED(rc = plGetSharedFontByType(&BuiltinFont::g_stdFont, PlSharedFontType_Standard)))
        goto close_serv;

    if (R_FAILED(rc = plGetSharedFontByType(&BuiltinFont::g_extFont, PlSharedFontType_NintendoExt)))
        goto close_serv;

    goto end;

close_serv:
    plExit();
end:
    return rc;
}

void BuiltinFont::finalize() {
    plExit();
}

PlFontData *BuiltinFont::getFont(u32 chr) const {
    if ((0x20 <= chr) && (chr <= 0x266b))
        return &BuiltinFont::g_stdFont;
    else if ((0xe000 <= chr) && (chr <= 0xe153))
        return &BuiltinFont::g_extFont;
    return nullptr;
}

Result FreetypeFont::initialize() {
    Result rc = 0;
    FT_Error ft_rc = 0;

    if (R_FAILED(rc = BuiltinFont::initialize()))
        goto finalize;

    if (R_FAILED(ft_rc = FT_Init_FreeType(&FreetypeFont::g_ftLib)))
        goto finalize;

    if (R_FAILED(ft_rc = FT_New_Memory_Face(FreetypeFont::g_ftLib, (FT_Bytes)BuiltinFont::g_stdFont.address, BuiltinFont::g_stdFont.size, 0, &FreetypeFont::g_stdFace)))
        goto close_ft;

    if (R_FAILED(ft_rc = FT_New_Memory_Face(FreetypeFont::g_ftLib, (FT_Bytes)BuiltinFont::g_extFont.address, BuiltinFont::g_extFont.size, 0, &FreetypeFont::g_extFace)))
        goto close_std_face;

    goto end;

close_std_face:
    FT_Done_Face(FreetypeFont::g_stdFace);
close_ft:
    FT_Done_FreeType(FreetypeFont::g_ftLib);
finalize:
    finalize();
end:

    return (Result)(rc | ft_rc);
}

void FreetypeFont::finalize() {
    FT_Done_Face(FreetypeFont::g_extFace);
    FT_Done_Face(FreetypeFont::g_stdFace);
    FT_Done_FreeType(FreetypeFont::g_ftLib);
    BuiltinFont::finalize();
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

Result StbFont::initialize() {
    Result rc = 0;

    if (R_FAILED(rc = BuiltinFont::initialize()))
        goto finalize;

    if (stbtt_InitFont(&StbFont::g_stdInfo, (u8 *)StbFont::g_stdFont.address, 0) == 0) {
        rc = 0xabcd;
        goto finalize;
    }

    if (stbtt_InitFont(&StbFont::g_extInfo, (u8 *)StbFont::g_extFont.address, 0) == 0) {
        rc = 0xabcde;
        goto finalize;
    }

    goto end;

finalize:
    finalize();
end:
    return rc;
}

void StbFont::finalize() {
    BuiltinFont::finalize();
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
