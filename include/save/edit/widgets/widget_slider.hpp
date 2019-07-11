#pragma once

#include "edizon.hpp"
#include "save/edit/widgets/widget.hpp"

namespace edz::save::edit::widget {

    class WidgetSlider : public Widget {
    public:
        WidgetSlider(std::string name);
        ~WidgetSlider();

        WidgetType getWidgetType();

    private:

    };

}