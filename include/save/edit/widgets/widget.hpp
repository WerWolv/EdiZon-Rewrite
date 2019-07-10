#pragma once

#include "edizon.hpp"

#include <string>
#include <vector>

namespace edz::save::edit::widget {

    enum class WidgetType {
        INTEGER,
        BOOLEAN,
        LIST,
        STRING,
        SLIDER,
        PROGRESS_BAR,
        COMMENT,
        IMAGE
    };

    enum class ArgumentType {
        INTEGER,
        FLOAT,
        BOOLEAN,
        STRING
    };

    template<typename T>
    struct Argument{
        std::string argName;
        T value;

        constexpr ArgumentType getType() {
            if constexpr (std::is_same<T, s128>::value)
                return INTEGER;
            else if constexpr (std::is_same<T, double>::value)
                return FLOAT;
            else if constexpr (std::is_same<T, bool>::value)
                return BOOLEAN;
            else if constexpr (std::is_same<T, std::string>::value)
                return STRING;
            else static_assert(false, "Invalid argument type!");
        }
    };

    typedef union {
        Argument<s128> intArg;
        Argument<double> floatArg;
        Argument<bool> boolArg;
        Argument<std::string> stringArg;
    } arg_t;

    class Widget {
    public:
        Widget();
        virtual ~Widget();

        virtual WidgetType getWidgetType() = 0;
        virtual View* getView() = 0;

        void addArgument(arg_t *argument);

    private:
        std::string m_name;
        std::string m_description;
        std::vector<arg_t*> m_arguments;
    };

}