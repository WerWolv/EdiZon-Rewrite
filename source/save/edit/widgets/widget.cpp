#include "save/edit/widgets/widget.hpp"

namespace edz::save::edit::widget {

    Widget::Widget(std::string name) : m_name(name) {
        this->m_widgetView = new ListItem(this->m_name, this->m_description);
    }

    Widget::~Widget() {
        delete this->m_widgetView;
    }


    void Widget::sendValueToScript(Arg arg) {

    }

    void Widget::setDescription(std::string description) {
        this->m_description = description;
    }

    void Widget::addArgument(std::string argumentName, Arg argument) {
        this->m_arguments.insert({ argumentName, argument });
    }

    ListItem* Widget::getView() {
        return this->m_widgetView;
    }

}