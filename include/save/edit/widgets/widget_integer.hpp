#pragma once

#include "edizon.hpp"
#include "save/edit/widgets/widget.hpp"

namespace edz::save::edit::widget {

    class WidgetInteger : public Widget {
    public:
        WidgetInteger(std::string name, s64 minValue, s64 maxValue);
        ~WidgetInteger();

        WidgetType getWidgetType();

    private:
        s64 m_currValue;
        s64 m_minValue;
        s64 m_maxValue;
    };

}