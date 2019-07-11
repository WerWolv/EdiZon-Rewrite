#pragma once

#include "edizon.hpp"
#include "save/edit/widgets/widget.hpp"

namespace edz::save::edit::widget {

    class WidgetBoolean : public Widget {
    public:
        WidgetBoolean(std::string name, Arg onValue, Arg offValue);
        ~WidgetBoolean();

        WidgetType getWidgetType();

    private:
        bool m_state;
        Arg m_onValue;
        Arg m_offValue;
    };

}