#pragma once

#include "edizon.hpp"
#include "save/edit/widgets/widget.hpp"

namespace edz::save::edit::widget {

    class WidgetComment : public Widget {
    public:
        WidgetComment(std::string name, std::string comment);
        ~WidgetComment();

        WidgetType getWidgetType();
        ListItem* getView();

    private:
        std::string m_comment;
    };

}