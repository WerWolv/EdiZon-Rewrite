#include "save/edit/widgets/widget_comment.hpp"
#include "helpers/utils.hpp"

namespace edz::save::edit::widget {

    WidgetComment::WidgetComment(std::string name, std::string comment) : Widget(name), m_comment(comment) {

    }

    WidgetComment::~WidgetComment() {

    }


    WidgetType WidgetComment::getWidgetType() {
        return WidgetType::COMMENT;
    }

    View* WidgetComment::getView() {
        if (this->m_widgetView == nullptr) {
            this->m_widgetView = new Label(LabelStyle::REGULAR, this->m_comment, true);
        }

        return this->m_widgetView;
    }

}
