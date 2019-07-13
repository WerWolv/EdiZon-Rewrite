#pragma once

#include "edizon.hpp"
#include "save/edit/widgets/widget.hpp"

namespace edz::save::edit::widget {

    class WidgetProgressbar : public Widget {
    public:
        WidgetProgressbar(std::string name);
        ~WidgetProgressbar();

        WidgetType getWidgetType();
        ListItem* getView();

    private:

    };

}