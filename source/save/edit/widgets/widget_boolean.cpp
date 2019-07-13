#include "save/edit/widgets/widget_boolean.hpp"
#include "helpers/utils.hpp"

namespace edz::save::edit::widget {

    WidgetBoolean::WidgetBoolean(std::string name, std::shared_ptr<widget::Arg> onValue, std::shared_ptr<widget::Arg> offValue) : Widget(name), m_onValue(onValue), m_offValue(offValue) {
        this->m_state = false;
    }

    WidgetBoolean::~WidgetBoolean() {

    }


    WidgetType WidgetBoolean::getWidgetType() {
        return WidgetType::BOOLEAN;
    }

    ListItem* WidgetBoolean::getView() {
        if (this->m_widgetView == nullptr) {
            this->m_widgetView = new ListItem(this->m_name, this->m_description);

            this->m_widgetView->setValue(this->m_state ? "On" : "Off", !this->m_state);

            this->m_widgetView->setClickListener([&](View *view){
                this->m_state = !this->m_state;
                this->m_widgetView->setValue(this->m_state ? "On" : "Off", !this->m_state);
            });
        }

        return this->m_widgetView;
    }

}
