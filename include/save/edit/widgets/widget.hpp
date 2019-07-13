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



    class Arg {
    public:
        template <typename T>
        static std::shared_ptr<widget::Arg> create(T value) {
            std::shared_ptr<widget::Arg> ret = std::make_shared<Arg>();

            if constexpr (std::is_integral<T>::value) {
                ret->intArg.value = value;
                ret->type = ArgumentType::INTEGER;
            }
            else if constexpr (std::is_floating_point<T>::value) {
                ret->floatArg.value = value;
                ret->type = ArgumentType::FLOAT;
            }
            else if constexpr (std::is_same<T, bool>::value) {
                ret->boolArg.value = value;
                ret->type = ArgumentType::BOOLEAN;
            }
            else if constexpr (std::is_same<T, std::string>::value) {
                ret->stringArg.value = value;
                ret->type = ArgumentType::STRING;
            }

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

        Argument<s128> intArg;
        Argument<double> floatArg;
        Argument<bool> boolArg;
        Argument<std::string> stringArg;

        ArgumentType type;
    };


    class Widget {
    public:
        Widget(std::string name);
        virtual ~Widget();

        virtual WidgetType getWidgetType() = 0;
        virtual View* getView() = 0;

        void sendValueToScript(std::shared_ptr<widget::Arg> arg);

        void setDescription(std::string description);
        void addArgument(std::string argumentName, std::shared_ptr<widget::Arg> argument);

    protected:
        std::string m_name;
        std::string m_description;
        std::map<std::string, std::shared_ptr<widget::Arg>> m_arguments;

        View *m_widgetView = nullptr;
    };

}