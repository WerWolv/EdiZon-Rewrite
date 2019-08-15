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
#include <vector>
#include <map>

#include "save/edit/script/script.hpp"
#include "save/edit/widgets/widget.hpp"

#define LLONG_MAX INT64_MAX
#define LLONG_MIN INT64_MIN
#define LUA_COMPAT_5_3

extern "C" {
    #define LUA_PATH_DEFAULT EDIZON_BASE_DIR "/scripts/?.lua;" \
                             EDIZON_BASE_DIR "/scripts/libs/?.lua;" 

    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

namespace edz::save::edit {

    class ScriptLua : public Script {
    public:
        ScriptLua(std::string path);
        ~ScriptLua();


        std::tuple<EResult, std::shared_ptr<widget::Arg>> getValue() override;
        EResult setValue(std::shared_ptr<widget::Arg> value) override;
        std::tuple<EResult, std::vector<u8>> getModifiedSaveData() override;

    private:
        lua_State *m_ctx;

        // Script callable functions
        int luaopen_edizon(lua_State *ctx);

        int getArgument(lua_State *ctx);
        int getDataAsBuffer(lua_State *ctx);
        int getDataAsString(lua_State *ctx);
    };

}