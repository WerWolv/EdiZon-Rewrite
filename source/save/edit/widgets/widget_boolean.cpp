#include "save/edit/widgets/widget_boolean.hpp"
#include "helpers/utils.hpp"

namespace edz::save::edit::widget {

    WidgetBoolean::WidgetBoolean(std::string name, Arg onValue, Arg offValue) : Widget(name), m_onValue(onValue), m_offValue(offValue) {
        this->m_widgetView->setValue(this->m_state ? "On" : "Off", this->m_state);

        this->m_widgetView->setClickListener([&](View *view){
            this->m_state = !this->m_state;
            this->m_widgetView->setValue(this->m_state ? "On" : "Off", this->m_state);
        });
    }

    WidgetBoolean::~WidgetBoolean() {

    }


    WidgetType WidgetBoolean::getWidgetType() {
        return WidgetType::BOOLEAN;
    }

}
