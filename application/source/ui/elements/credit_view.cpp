/**
 * Copyright (C) 2019 - 2020 WerWolv
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

#include "ui/fonts.hpp"
#include "ui/elements/credit_view.hpp"
#include "cheat/cheat.hpp"

#include "helpers/utils.hpp"

namespace edz::ui::element {

    CreditView::CreditView() {

    }

    CreditView::~CreditView() {

    }


    void CreditView::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, brls::Style *style, brls::FrameContext *ctx) {       
        static const std::string credits[] = {
            "edz.gui.main.about.credits.1"_lang,
            "edz.gui.main.about.credits.2"_lang,
            "edz.gui.main.about.credits.3"_lang,
            "edz.gui.main.about.credits.4"_lang,
            "edz.gui.main.about.credits.5"_lang
        };

        nvgFontFaceId(vg, ctx->fontStash->regular);
        nvgFontSize(vg, 16);
        nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);

        u8 lineNumber = 0;
        for (auto &credit : credits) {
            nvgBeginPath(vg);
            nvgFillColor(vg, a(ctx->theme->descriptionColor));
            nvgText(vg, x + 10, y + 10 + lineNumber * 26, credit.c_str(), nullptr);
            lineNumber++;
        }
    }

    void CreditView::layout(NVGcontext* vg, brls::Style *style, brls::FontStash *stash) {
        this->height = 120;
    }

    brls::View* CreditView::requestFocus(brls::FocusDirection direction, brls::View *oldFocus, bool fromUp) {        
        return nullptr;
    }

    void CreditView::drawHighlight(NVGcontext* vg, brls::ThemeValues* theme, float alpha, brls::Style* style, bool background) {

    }

}