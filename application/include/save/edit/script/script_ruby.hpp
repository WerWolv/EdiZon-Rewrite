#pragma once

#include "edizon.hpp"
#include <string>
#include <vector>
#include <map>

#include "save/edit/script/script.hpp"
#include "save/edit/widgets/widget.hpp"

#include <mruby.h>

namespace edz::save::edit {

    class ScriptRuby : public Script {
    public:
        ScriptRuby(std::string scriptName);
        ~ScriptRuby();


        std::shared_ptr<widget::Arg> getValue();
        void setValue(std::shared_ptr<widget::Arg> value);

        std::shared_ptr<widget::Arg> callFunction(std::string funcName, std::vector<std::shared_ptr<widget::Arg>> params);
        std::shared_ptr<widget::Arg> evaluate(std::string code);

    private:
        mrb_context m_ctx;
    };

}