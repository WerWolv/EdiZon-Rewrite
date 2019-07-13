#pragma once

#include "edizon.hpp"
#include "save/edit/widgets/widget.hpp"

namespace edz::save::edit::widget {

    class WidgetBoolean : public Widget {
    public:
        WidgetBoolean(std::string name, std::shared_ptr<widget::Arg> onValue, std::shared_ptr<widget::Arg> offValue);
        ~WidgetBoolean();

        WidgetType getWidgetType();
        ListItem* getView();

    private:
        bool m_state;
        std::shared_ptr<widget::Arg> m_onValue;
        std::shared_ptr<widget::Arg> m_offValue;
    };

}