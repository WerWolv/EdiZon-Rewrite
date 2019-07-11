#pragma once

#include "edizon.hpp"
#include "save/edit/widgets/widget.hpp"
#include <vector>

namespace edz::save::edit::widget {

    class WidgetList : public Widget {
    public:
        WidgetList(std::string name, std::vector<std::string> displayStrings, std::vector<Arg> arguments);
        ~WidgetList();

        WidgetType getWidgetType();

    private:

    };

}