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

namespace edz::save::edit::widget {

    enum class WidgetType {
        INTEGER,
        BOOLEAN,
        LIST,
        STRING,
        SLIDER,
        PROGRESS_BAR,
        COMMENT
    };



    class Arg {
    public:
        template <typename T>
        static std::shared_ptr<widget::Arg> create(std::string name, T value) {
            std::shared_ptr<widget::Arg> ret = std::make_shared<Arg>();

            if constexpr (std::is_integral<T>::value) {
                ret->intArg = value;
                ret->type = ArgumentType::INTEGER;
            }
            else if constexpr (std::is_floating_point<T>::value) {
                ret->floatArg = value;
                ret->type = ArgumentType::FLOAT;
            }
            else if constexpr (std::is_same<T, bool>::value) {
                ret->boolArg = value;
                ret->type = ArgumentType::BOOLEAN;
            }
            else if constexpr (std::is_same<T, std::string>::value) {
                ret->stringArg = value;
                ret->type = ArgumentType::STRING;
            }

            ret->name = name;

            return ret;
        }

        enum class ArgumentType {
            INVALID,
            INTEGER,
            FLOAT,
            BOOLEAN,
            STRING
        };

        s128 intArg = 0;
        double floatArg = 0;
        bool boolArg = false;
        std::string stringArg;

        ArgumentType type = ArgumentType::INVALID;
        std::string name;
    };


    class Widget {
    public:
        Widget(std::string name);
        virtual ~Widget();

        virtual WidgetType getWidgetType() = 0;
        virtual View* getView() = 0;

        virtual void sendValueToScript(std::shared_ptr<widget::Arg> arg) final;

        virtual void setDescription(std::string description) final;
        virtual void addArgument(std::string argumentName, std::shared_ptr<widget::Arg> argument) final;

    protected:
        std::string m_name;
        std::string m_description;
        std::map<std::string, std::shared_ptr<widget::Arg>> m_arguments;

        View *m_widgetView = nullptr;
    };

}