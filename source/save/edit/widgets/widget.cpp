#include "save/edit/widgets/widget.hpp"

namespace edz::save::edit::widget {

    Widget::Widget(std::string name) : m_name(name) {
        this->m_widgetView = nullptr;
    }

    Widget::~Widget() {
        if (this->m_widgetView != nullptr)
            delete this->m_widgetView;
    }


    void Widget::sendValueToScript(std::shared_ptr<widget::Arg> arg) {

    }

    void Widget::setDescription(std::string description) {
        this->m_description = description;
    }

    void Widget::addArgument(std::string argumentName, std::shared_ptr<widget::Arg> argument) {
        this->m_arguments.insert({ argumentName, argument });
    }


}