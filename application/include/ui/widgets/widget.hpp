/**
 * Copyright (C) 2019 WerWolv
 * 
 * This file is part of EdiZon.
 * 
 * EdiZon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 * 
 * EdiZon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with EdiZon.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <edizon.hpp>

#include <string>
#include <map>
#include <memory>
#include <variant>

namespace brls {
    class View;
}

namespace edz::save::edit {
    class Script;
}

namespace edz::ui::widget {

    enum class WidgetType {
        INTEGER,
        BOOLEAN,
        LIST,
        STRING,
        SLIDER,
        PROGRESS_BAR,
        COMMENT
    };

    using Argument = std::variant<s128, double, bool, std::string>;
    using NamedArgument = std::pair<std::string, Argument>;

    class Widget {
    public:
        Widget(std::string name);
        virtual ~Widget();

        virtual WidgetType getWidgetType() = 0;
        virtual brls::View* getView() = 0;

        virtual void sendValueToScript(Argument arg) final;

        virtual void setDescription(std::string description) final;
        virtual void addArgument(std::string argumentName, Argument argument) final;

        virtual void setScript(edz::save::edit::Script *script) final;

    protected:
        std::string m_name;
        std::string m_description;
        std::map<std::string, Argument> m_arguments;

        brls::View *m_widgetView = nullptr;

        edz::save::edit::Script *m_script;
    };

}