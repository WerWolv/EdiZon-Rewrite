#include "save/edit/widgets/widget_list.hpp"
#include "helpers/utils.hpp"

namespace edz::save::edit::widget {

    WidgetList::WidgetList(std::string name, std::vector<std::string> displayStrings, std::vector<std::shared_ptr<widget::Arg>> arguments) : Widget(name), m_displayStrings(displayStrings), m_arguments(arguments) {
        this->m_currValue = 0;
    }

    WidgetList::~WidgetList() {

    }


    WidgetType WidgetList::getWidgetType() {
        return WidgetType::LIST;
    }

    View* WidgetList::getView() {
        if (this->m_widgetView == nullptr) {
            this->m_widgetView = new ListItem(this->m_name, this->m_description);
            ListItem *listItem = reinterpret_cast<ListItem*>(this->m_widgetView);

            listItem->setValue(this->m_displayStrings[this->m_currValue]);

            listItem->setClickListener([&](View *view){
                SelectView::open(this->m_name, this->m_displayStrings, [&](s32 selectedItem){
                    if (selectedItem == -1) return;

                    this->m_currValue = selectedItem;
                    listItem->setValue(this->m_displayStrings[selectedItem]);
                }, this->m_currValue);
            });
        }

        return this->m_widgetView;
    }

}
