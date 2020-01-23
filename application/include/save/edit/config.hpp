/**
 * Copyright (C) 2020 WerWolv
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

#include "save/title.hpp"
#include "ui/widgets/widget.hpp"

#include <string>
#include <vector>
#include <map>

#include <nlohmann/json_fwd.hpp>

namespace brls {
    class TabFrame;
}

namespace edz::save::edit {

    enum class ScriptLanguage {
        LUA,
        PYTHON
    };

    typedef struct {
        std::vector<ui::widget::Widget*> widgets;
    } WidgetCategory;

    typedef struct {
        std::vector<std::string> saveFilePaths;
        std::vector<std::string> saveFileNames;

        std::map<std::string, WidgetCategory> categories;
    } FileConfig;

    class Config {
    public:
        Config(std::unique_ptr<Title> &title, std::unique_ptr<Account> &account);
        ~Config();

        EResult load();
        void createUI(brls::TabFrame *tabFrame);

        std::string getAuthor();
        std::string getDescription();
        ScriptLanguage getScriptLanguage();
        std::string getScript();
        bool isBeta();
        std::vector<std::string> getSaveFilePaths();
        void setScript(save::edit::Script *script);

    private:
        std::unique_ptr<Title> &m_title;
        std::unique_ptr<Account> &m_account;

        std::string m_author;
        std::string m_description;
        ScriptLanguage m_scriptLanguage;
        std::string m_script;
        bool m_isBeta;

        std::map<u16, FileConfig> m_fileConfigs;

        bool jsonExists(nlohmann::json &j, std::string key);

        void parseMetadata(nlohmann::json &j);
        void parseVersionSpecificMetadata(nlohmann::json &j, u16 fileNum);
        void parseWidgets(nlohmann::json &j, u16 fileNum);
    };

}