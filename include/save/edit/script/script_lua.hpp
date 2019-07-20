#pragma once

#include "edizon.hpp"
#include <string>
#include <vector>
#include <map>

#include "save/edit/script/script.hpp"
#include "save/edit/widgets/widget.hpp"

#define LLONG_MAX INT64_MAX
#define LLONG_MIN INT64_MIN
#define LUA_COMPAT_5_3

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

namespace edz::save::edit {

    class ScriptLua : public Script {
    public:
        ScriptLua(std::string scriptName);
        ~ScriptLua();


        std::shared_ptr<widget::Arg> getValue();
        void setValue(std::shared_ptr<widget::Arg> value);
        std::vector<u8> getModifiedSaveFileData();

    private:
        lua_State *m_ctx;

        // Script callable functions
        int getArgument(lua_State *ctx);
        int getDataAsBuffer(lua_State *ctx);
        int getDataAsString(lua_State *ctx);
    };

}