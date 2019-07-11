#pragma once

#include "edizon.hpp"

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

    class _Arg {
    public:
        template <typename T>
        static Arg create(T value) {
            Arg ret = std::make_shared<Arg>();

            if (std::is_integral<T>::value) {
                ret.arg.intArg = value;
                ret.type = ArgumentType::INTEGER;
            }
            else if (std::is_floating_point<T>::value) {
                ret.arg.floatArg = value;
                ret.type = ArgumentType::FLOAT;
            }
            else if (std::is_same<T, bool>::value) {
                ret.arg.boolArg = value;
                ret.type = ArgumentType::BOOLEAN;
            }
            else if (std::is_same<T, std::string>::value) {
                ret.arg.stringArg = value;
                ret.type = ArgumentType::STRING;
            }
            else static_assert(false, "Invalid argument type!");

            return ret;
        }

    private:
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
        };

        union {
            Argument<s128> intArg;
            Argument<double> floatArg;
            Argument<bool> boolArg;
            Argument<std::string> stringArg;
        } arg;

        ArgumentType type;

        _Arg();
    };

    typedef std::shared_ptr<_Arg> Arg;

    class Widget {
    public:
        Widget(std::string name);
        virtual ~Widget();

        virtual WidgetType getWidgetType() = 0;

        void sendValueToScript(Arg arg);

        void setDescription(std::string description);
        void addArgument(std::string argumentName, Arg argument);
        ListItem* getView();

    protected:
        std::string m_name;
        std::string m_description;
        std::map<std::string, Arg> m_arguments;

        ListItem *m_widgetView = nullptr;
    };

}