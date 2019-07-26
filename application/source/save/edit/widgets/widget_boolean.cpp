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

    View* WidgetBoolean::getView() {
        if (this->m_widgetView == nullptr) {
            this->m_widgetView = new ListItem(this->m_name, this->m_description);
            ListItem *listItem = reinterpret_cast<ListItem*>(this->m_widgetView);

            listItem->setValue(this->m_state ? edz::LangEntry("edz.widget.boolean.on").get() : edz::LangEntry("edz.widget.boolean.off").get(), !this->m_state);

            listItem->setClickListener([&](View *view){
                this->m_state = !this->m_state;
                listItem->setValue(this->m_state ? edz::LangEntry("edz.widget.boolean.on").get() : edz::LangEntry("edz.widget.boolean.off").get(), !this->m_state);
            });
        }

        return this->m_widgetView;
    }

}
