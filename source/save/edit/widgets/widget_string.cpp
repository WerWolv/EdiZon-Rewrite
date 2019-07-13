#include "save/edit/widgets/widget_string.hpp"
#include "helpers/utils.hpp"

namespace edz::save::edit::widget {

    WidgetString::WidgetString(std::string name, u32 minLength, u32 maxLength) : Widget(name), m_minLength(minLength), m_maxLength(maxLength) {
        this->m_currValue = "";
    }

    WidgetString::~WidgetString() {

    }


    WidgetType WidgetString::getWidgetType() {
        return WidgetType::STRING;
    }

    View* WidgetString::getView() {
        if (this->m_widgetView == nullptr) {
            this->m_widgetView = new ListItem(this->m_name, this->m_description);
            ListItem *listItem = reinterpret_cast<ListItem*>(this->m_widgetView);
            
            listItem->setValue(this->m_currValue);

            listItem->setClickListener([&](View *view){
                edz::helper::askSwkbdText([&](std::string str){
                    if (str.length() < this->m_minLength) return;

                    this->m_currValue = str;
                    listItem->setValue(this->m_currValue);

                }, edz::LangEntry("edz.widget.string.title").get(), edz::LangEntry("edz.widget.string.subtitle").get(), this->m_maxLength, this->m_currValue);
            });
        }

        return this->m_widgetView;
    }

}
