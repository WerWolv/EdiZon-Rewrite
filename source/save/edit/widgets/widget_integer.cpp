#include "save/edit/widgets/widget_integer.hpp"
#include "helpers/utils.hpp"
#include <math.h>

namespace edz::save::edit::widget {

    WidgetInteger::WidgetInteger(std::string name, s64 minValue, s64 maxValue) : Widget(name), m_minValue(minValue), m_maxValue(maxValue) {
        this->m_currValue = minValue;
    }

    WidgetInteger::~WidgetInteger() {

    }


    WidgetType WidgetInteger::getWidgetType() {
        return WidgetType::INTEGER;
    }

    ListItem* WidgetInteger::getView() {
        if (this->m_widgetView == nullptr) {
            this->m_widgetView = new ListItem(this->m_name, this->m_description);

            this->m_widgetView->setValue(std::to_string(this->m_currValue));

            this->m_widgetView->setClickListener([&](View *view){
                edz::helper::askSwkbdNumber([&](std::string str){

                    s64 newValue = std::stoll(str);
                    this->m_currValue = std::max(this->m_minValue, std::min(this->m_maxValue, newValue));
                    this->m_widgetView->setValue(std::to_string(this->m_currValue));

                }, "Enter a value", "This will be the new value this value will be set to.", "-", "", std::floor(std::log10(this->m_maxValue)) + 1, std::to_string(this->m_currValue));
            });
            }

        return this->m_widgetView;
    }

}
