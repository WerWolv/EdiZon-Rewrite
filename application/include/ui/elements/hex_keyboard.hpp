/**
 * Copyright (C) 2020 WerWolv
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

#include <edizon.hpp>
#include <borealis.hpp>

#include <string>

namespace edz::ui::element {

    typedef std::function<void(u8 *value, size_t size)> KeyboardListener;

    class KeyboardButton : public brls::Button {
    public:
        enum class ButtonColor { White, LightGray, Gray, Colored };

        KeyboardButton(std::string label, ButtonColor buttonColor) : brls::Button(brls::ButtonStyle::PLAIN, label), m_buttonColor(buttonColor) { }

        void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) override {
            switch (this->m_buttonColor) {
                case ButtonColor::White:
                    nvgFillColor(vg, a(ctx->theme->textColor));
                    break;
                case ButtonColor::LightGray:
                    nvgFillColor(vg, a(ctx->theme->descriptionColor));
                    break;
                case ButtonColor::Gray:
                    nvgFillColor(vg, a(ctx->theme->buttonPlainDisabledBackgroundColor));
                    break;
                case ButtonColor::Colored:
                    nvgFillColor(vg, a(ctx->theme->buttonPlainEnabledBackgroundColor));
                    break;
            }

            nvgBeginPath(vg);
            nvgRect(vg, x, y, width, height);
            nvgFill(vg);
        }

    private:
        ButtonColor m_buttonColor;
    };

    class HexKeyboard : public brls::View {
    public:
        ~HexKeyboard();

        void draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) override;
        void layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash) override;
        View* requestFocus(brls::FocusDirection direction, View* oldFocus, bool fromUp = false) override;
        bool onCancel();
        void show(std::function<void(void)> cb, bool animate = true, brls::ViewAnimation animation = brls::ViewAnimation::FADE) override;
        void willAppear() override;

        static void open(std::string title, KeyboardListener listener, size_t maxInputSize);

        bool isTranslucent() override {
            return true || brls::View::isTranslucent();
        }

    protected:
        unsigned getShowAnimationDuration(brls::ViewAnimation animation) override;

        bool animateHint() override {
            return true;
        }

    private:
        HexKeyboard(std::string title, KeyboardListener listener, size_t maxInputSize);

        float m_topOffset;
        KeyboardListener m_listener;

        brls::Hint *m_hint;
        brls::Button *m_buttons[24] = { nullptr };
    };

} // namespace brls
