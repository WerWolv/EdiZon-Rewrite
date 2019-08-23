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

#include "save/edit/script/script_lua.hpp"

namespace edz::save::edit {

    using mem_func = int (ScriptLua::*)(lua_State *s);
    template <mem_func func>
    int dispatch(lua_State *s) {
        ScriptLua *ptr = *static_cast<ScriptLua**>(lua_getextraspace(s));
        return ((*ptr).*func)(s);
    }

    ScriptLua::ScriptLua(std::string path) : Script(path) {
        this->m_ctx = luaL_newstate(); 
        luaL_openlibs(this->m_ctx);

        *static_cast<ScriptLua**>(lua_getextraspace(this->m_ctx)) = this;

        // Make it possible to include the edizon module using require('edizon')
        luaL_getsubtable(this->m_ctx, LUA_REGISTRYINDEX, LUA_PRELOAD_TABLE);
        lua_pushcfunction(this->m_ctx, &dispatch<&ScriptLua::luaopen_edizon>);
        lua_setfield(this->m_ctx, -2, "edizon");
        lua_pop(this->m_ctx, 1);

        if (luaL_loadfile(this->m_ctx, path.c_str()) != 0) {
            error("Failed to load Lua script file");
            return;
        }

        if (lua_pcall(this->m_ctx, 0, 0, 0)) {
            error("Failed to execute Lua script's global scope: %s", lua_tostring(this->m_ctx, -1));
            return;
        }

        info("Lua script successfully loaded!");
    }

    ScriptLua::~ScriptLua() {
        if (this->m_ctx != nullptr)
            lua_close(this->m_ctx);
    }


    std::tuple<EResult, std::shared_ptr<ui::widget::Arg>> ScriptLua::getValue() {
        std::shared_ptr<ui::widget::Arg> out;

        lua_getglobal(this->m_ctx, "getValue");
        if (lua_pcall(this->m_ctx, 0, 1, 0)) {
            error("Lua script's getValue function failed: %s", lua_tostring(this->m_ctx, -1));
            return { ResultEdzScriptRuntimeError, out };  // Return empty argument
        }


        if (lua_isinteger(this->m_ctx, -1))
            out = ui::widget::Arg::create("value", lua_tointeger(this->m_ctx, -1));
        else if (lua_isnumber(this->m_ctx, -1))
            out = ui::widget::Arg::create("value", lua_tonumber(this->m_ctx, -1));
        else if (lua_isboolean(this->m_ctx, -1))
            out = ui::widget::Arg::create("value", lua_toboolean(this->m_ctx, -1));
        else if (lua_isstring(this->m_ctx, -1))
            out = ui::widget::Arg::create("value", lua_tostring(this->m_ctx, -1));
        else error("Invalid value returned from Lua script's getValue!");

        lua_pop(this->m_ctx, 1);

        return { ResultSuccess, out };
    }

    EResult ScriptLua::setValue(std::shared_ptr<ui::widget::Arg> value) {
        lua_getglobal(this->m_ctx, "setValue");

        switch(value->type) {
            case ui::widget::Arg::ArgumentType::INTEGER:
                lua_pushinteger(this->m_ctx, value->intArg);
                break;
            case ui::widget::Arg::ArgumentType::FLOAT:
                lua_pushnumber(this->m_ctx, value->floatArg);
                break;
            case ui::widget::Arg::ArgumentType::BOOLEAN:
                lua_pushboolean(this->m_ctx, value->boolArg);
                break;            
            case ui::widget::Arg::ArgumentType::STRING:
                lua_pushstring(this->m_ctx, value->stringArg.c_str());
                break;
        }

        if (lua_pcall(this->m_ctx, 1, 0, 0)) {
            error("Lua script's setValue function failed: %s", lua_tostring(this->m_ctx, -1));
            return ResultEdzScriptRuntimeError;
        }

        return ResultSuccess;
    }

    std::tuple<EResult, std::vector<u8>> ScriptLua::getModifiedSaveData() {
        std::vector<u8> out;

        lua_getglobal(this->m_ctx, "getModifiedSaveData");

        if (lua_pcall(this->m_ctx, 0, 1, 0)) {
            error("Lua script's getModifiedSaveData function failed: %s", lua_tostring(this->m_ctx, -1));
            return { ResultEdzScriptRuntimeError, out }; // Return empty buffer
        }

        lua_pushnil(this->m_ctx);

        while (lua_next(this->m_ctx, 1)) {
            out.push_back(lua_tointeger(this->m_ctx, -1));
            lua_pop(this->m_ctx, 1);
        }

        lua_pop(this->m_ctx, 1);

        return { ResultSuccess, out };
    }


    int ScriptLua::luaopen_edizon(lua_State *ctx) {
        const luaL_Reg regs[] {
            { "getDataAsBuffer", &dispatch<&ScriptLua::getDataAsBuffer> },
            { "getDataAsString", &dispatch<&ScriptLua::getDataAsString> },
            { "getArgument",     &dispatch<&ScriptLua::getArgument>     },
            { nullptr, nullptr }
        };

        luaL_newlib(ctx, regs);

        return 1;
    }

    int ScriptLua::getArgument(lua_State *ctx) {
        std::string argName = std::string(lua_tostring(ctx, -1), lua_strlen(ctx, -1));

        std::shared_ptr<ui::widget::Arg> arg = this->m_arguments.at(argName);

        if (arg == nullptr) {
            lua_pushnil(ctx);
            return 1;
        }

        switch(arg->type) {
            case ui::widget::Arg::ArgumentType::INTEGER:
                lua_pushinteger(ctx, arg->intArg);
                break;
            case ui::widget::Arg::ArgumentType::FLOAT:
                lua_pushnumber(ctx, arg->floatArg);
                break;
            case ui::widget::Arg::ArgumentType::BOOLEAN:
                lua_pushboolean(ctx, arg->boolArg);
                break;            
            case ui::widget::Arg::ArgumentType::STRING:
                lua_pushstring(ctx, arg->stringArg.c_str());
                break;
        }

        return 1;
    }

    int ScriptLua::getDataAsBuffer(lua_State *ctx) {
        lua_createtable(ctx, this->m_saveSize, 0);

        for (u64 i = 0; i < this->m_saveSize; i++) {
            lua_pushinteger(ctx, i + 1);
            lua_pushinteger(ctx, this->m_saveData[i]);
            lua_settable(ctx, -3);
        }

        return 1;
    }

    int ScriptLua::getDataAsString(lua_State *ctx) {
        std::string saveDataString = std::string(reinterpret_cast<char*>(this->m_saveData), this->m_saveSize);
        saveDataString += "\x00";

        lua_pushstring(ctx, saveDataString.c_str());

        return 1;
    }

}
