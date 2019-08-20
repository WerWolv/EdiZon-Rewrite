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

#include "save/edit/editor.hpp"

#include "helpers/file.hpp"

#include "save/edit/script/script_lua.hpp"
#include "save/edit/script/script_python.hpp"

namespace edz::save::edit {

    Editor::Editor() {

    }

    Editor::~Editor() {

    }


    std::tuple<EResult, Config*, Script*> Editor::load(Title *title, Account *account, std::string saveFilePath) {
        if (Editor::s_config != nullptr || Editor::s_script != nullptr)
            return { ResultEdzEditorAlreadyLoaded, nullptr, nullptr };

        {
            edz::hlp::File configFile(EDIZON_BASE_DIR "/configs/" + title->getIDString() + ".json");

            if (!configFile.exists())
                return { ResultEdzEditorNoConfigFound, nullptr, nullptr };
        }


        Editor::s_config = new edz::save::edit::Config(title, account);

        if (EResult res = Editor::s_config->load(); res.failed()) {
            delete Editor::s_config;
            Editor::s_config = nullptr;

            return { res, nullptr, nullptr };
        }

        {
            edz::hlp::File scriptFile(EDIZON_BASE_DIR "/scripts/" + Editor::s_config->getScript());

            if (!scriptFile.exists())  {
                delete Editor::s_config;
                Editor::s_config = nullptr;

                return { ResultEdzEditorNoScriptFound, nullptr, nullptr };
            }
        }

        if (Editor::s_config->getScriptLanguage() == ScriptLanguage::LUA)
            Editor::s_script = new ScriptLua(EDIZON_BASE_DIR "/scripts/" + Editor::s_config->getScript());
        else if (Editor::s_config->getScriptLanguage() == ScriptLanguage::PYTHON)
            Editor::s_script = new ScriptPython(EDIZON_BASE_DIR "/scripts/" + Editor::s_config->getScript());
        
        Editor::s_currentSaveFile = new edz::hlp::File(saveFilePath);
        
        size_t saveDataSize = Editor::s_currentSaveFile->size();
        std::vector<u8> saveData;

        saveData.reserve(saveDataSize);

        Editor::s_currentSaveFile->read(&saveData[0], saveDataSize);
        Editor::s_script->setSaveData(&saveData[0], saveDataSize);

        return { ResultSuccess, Editor::s_config, Editor::s_script };

    }

    EResult Editor::store() {
        if (Editor::s_config == nullptr || Editor::s_script == nullptr)
            return ResultEdzEditorNotLoaded;

        auto [res, saveData] = Editor::s_script->getModifiedSaveData();

        if (res.failed()) {
            delete Editor::s_config;
            delete Editor::s_script;
            delete Editor::s_currentSaveFile;

            return ResultEdzEditorStoreFailed;
        }

        Editor::s_currentSaveFile->write(&saveData[0], saveData.size());

        return ResultSuccess;
    }

}