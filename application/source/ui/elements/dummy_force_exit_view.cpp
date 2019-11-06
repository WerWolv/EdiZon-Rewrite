/**
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

#include "ui/elements/dummy_force_exit_view.hpp"

namespace edz::ui::element {

    DummyForceExitView::DummyForceExitView() {

    }

    DummyForceExitView::~DummyForceExitView() {
        brls::Application::quit();
    }

    brls::View* DummyForceExitView::requestFocus(brls::FocusDirection direction, brls::View *oldFocus, bool fromUp) {        
        return nullptr;
    }

    void DummyForceExitView::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, brls::Style* style, brls::FrameContext* ctx) {

    }

}