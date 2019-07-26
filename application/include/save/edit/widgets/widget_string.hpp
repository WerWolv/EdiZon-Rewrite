#pragma once

#include "edizon.hpp"
#include "save/edit/widgets/widget.hpp"

namespace edz::save::edit::widget {

    class WidgetString : public Widget {
    public:
        WidgetString(std::string name, u32 minLength, u32 maxLength);
        ~WidgetString();

        WidgetType getWidgetType();
        View* getView();

    private:
        std::string m_currValue;
        u32 m_minLength;
        u32 m_maxLength;
    };

}