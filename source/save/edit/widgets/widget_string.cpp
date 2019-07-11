#include "save/edit/widgets/widget_string.hpp"
#include "helpers/utils.hpp"

namespace edz::save::edit::widget {

    WidgetString::WidgetString(std::string name, u32 minLength, u32 maxLength) : Widget(name), m_minLength(minLength), m_maxLength(maxLength) {
        this->m_widgetView->setValue(this->m_currValue);

        this->m_widgetView->setClickListener([&](View *view){
            edz::helper::askSwkbdText([&](std::string str){
                if (str.length() < this->m_minLength) return;

                this->m_currValue = str;
                this->m_widgetView->setValue(this->m_currValue);

            }, "Enter a value", "This will be the new value this value will be set to.", this->m_maxLength, this->m_currValue);
        });
    }

    WidgetString::~WidgetString() {

    }


    WidgetType WidgetString::getWidgetType() {
        return WidgetType::STRING;
    }

}
