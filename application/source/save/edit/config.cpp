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

#include "save/edit/config.hpp"

#include "save/save_data.hpp"


#include <fstream>
#include <regex>
#include <optional>

#include "ui/widgets/widget_integer.hpp"
#include "ui/widgets/widget_boolean.hpp"
#include "ui/widgets/widget_string.hpp"
#include "ui/widgets/widget_list.hpp"
#include "ui/widgets/widget_slider.hpp"
#include "ui/widgets/widget_progressbar.hpp"
#include "ui/widgets/widget_comment.hpp"

#include <nlohmann/json.hpp>
#include <borealis.hpp>

#define CONFIG_PATH EDIZON_BASE_DIR"/configs/"

using json = nlohmann::json;
using edz::ui::widget::Argument;
using edz::ui::widget::NamedArgument;

namespace edz::save::edit {

    NamedArgument jsonToArg(std::string argName, json::value_type jsonValue) {
        if (jsonValue.type() == json::value_t::number_integer || jsonValue.type() == json::value_t::number_unsigned)
            return std::make_pair(argName, static_cast<s128>(jsonValue.get<s64>()));
        else if (jsonValue.type() == json::value_t::number_float)
            return std::make_pair(argName, jsonValue.get<double>());
        else if (jsonValue.type() == json::value_t::boolean)
            return std::make_pair(argName, jsonValue.get<bool>());
        else if (jsonValue.type() == json::value_t::string)
            return std::make_pair(argName, jsonValue.get<std::string>());

        return std::make_pair(argName, s128());
    }

    Argument jsonToArg(json::value_type jsonValue) {
        if (jsonValue.type() == json::value_t::number_integer || jsonValue.type() == json::value_t::number_unsigned)
            return static_cast<s128>(jsonValue.get<s64>());
        else if (jsonValue.type() == json::value_t::number_float)
            return jsonValue.get<double>();
        else if (jsonValue.type() == json::value_t::boolean)
            return jsonValue.get<bool>();
        else if (jsonValue.type() == json::value_t::string)
            return jsonValue.get<std::string>();

        return s128();
    }
    
    Config::Config(std::unique_ptr<Title> &title, std::unique_ptr<Account> &account) : m_title(title), m_account(account) {

    }

    Config::~Config() {

    }


    EResult Config::load() {
        
        try {
            std::ifstream configFile(EDIZON_BASE_DIR "/configs/" + this->m_title->getIDString() + ".json");
            if (!configFile.is_open()) {
                // File couldn't be opened, no config file present
                return ResultEdzEditorNoConfigFound;
            }

            // Parse config file
            json config;
            configFile >> config;

            // Load basic config information
            parseMetadata(config);


            if (!jsonExists(config, this->m_title->getVersionString()) && !jsonExists(config, "all")) {
                // No save editor for this game version configured
                return ResultEdzEditorConfigOutdated;
            }

            

            json configVersionMetadata;

            if (jsonExists(config, this->m_title->getVersionString()))
                configVersionMetadata = config[this->m_title->getVersionString()];
            else configVersionMetadata = config["all"];

            u16 fileNum = 0;
            for (auto &fileDefinitions : configVersionMetadata) {
                parseVersionSpecificMetadata(fileDefinitions, fileNum);

                if (jsonExists(fileDefinitions, "items"))
                    parseWidgets(fileDefinitions["items"], fileNum);
                else throw json::parse_error::create(100, 0, "No widgets specified");

                fileNum++;
            }

        } catch (json::parse_error &e) {
            return ResultEdzEditorScriptSyntaxError;
        }

        return ResultSuccess;
    }
    

    bool Config::jsonExists(json &j, std::string key) {
        return j.find(key) != j.end();
    }


    void Config::parseMetadata(json &j) {
        this->m_author = j["author"];
        this->m_description = j["description"];
        this->m_isBeta = j["beta"];
        this->m_script = j["script"];

        if (j["scriptLanguage"] == "lua") 
            this->m_scriptLanguage = ScriptLanguage::LUA;
        else if (j["scriptLanguage"] == "py" || j["scriptLanguage"] == "python")
            this->m_scriptLanguage = ScriptLanguage::PYTHON;
    }

    void Config::parseVersionSpecificMetadata(json &j, u16 fileNum) {
        auto saveFileSystem = new SaveFileSystem(this->m_title, this->m_account);

        std::vector<std::string> pathRegex = j["pathRegex"];
        std::string fileNameRegex = j["fileNameRegex"];

        std::vector<std::string> matchingPaths;

        bool inRootDirectory = true;
        u8 folderDepth = 0;

        FileConfig fileConfig;

        // Parse save file path regex into the actual paths
        for (std::string folderRegex : pathRegex) {
            std::vector<std::string> tempMatchingPaths;

            if (inRootDirectory) {
                // Handle root directory
                saveFileSystem->getSaveFolder().foreach([&](struct dirent* entry){
                    if (entry->d_type == DT_DIR) {
                        if (std::regex_match(std::string(entry->d_name), std::regex(folderRegex)))
                            tempMatchingPaths.push_back(std::string(entry->d_name) + "/");
                    }
                });
                inRootDirectory = false;

            } else {
                // Handle all further directories
                for (std::string path : matchingPaths) {
                    saveFileSystem->getSaveFolder().getFolders().find(path)->second.foreach([&](struct dirent* entry){
                        if (entry->d_type == DT_DIR) {
                            if (std::regex_match(std::string(entry->d_name), std::regex(folderRegex))) {
                                if (folderDepth < (pathRegex.size() - 1))
                                    tempMatchingPaths.push_back(path + "/" + entry->d_name);
                                else // We reached the last level. Store the full paths for loading later
                                    fileConfig.saveFilePaths.push_back(path + "/" + entry->d_name);
                            }
                        }
                    });
                }
            }

            folderDepth++;
            matchingPaths = tempMatchingPaths;
        }

        
        for (std::string path : fileConfig.saveFilePaths) {
            edz::hlp::Folder saveFileFolder(saveFileSystem->getSaveFolder().path() + path);

            for (auto &[fileName, file] : saveFileFolder.getFiles())
                if (std::regex_match(fileName, std::regex(fileNameRegex)))
                    fileConfig.saveFileNames.push_back(path + "/" + fileName);
        }

        m_fileConfigs[fileNum] = fileConfig;
    }

    void Config::parseWidgets(json &j, u16 fileNum) {
        for (auto itemDescription : j) {
            auto widgetDescription = itemDescription["widget"];
            ui::widget::Widget *widget = nullptr;

            if (widgetDescription["type"] == "int") {
                if (widgetDescription["minValue"].type() != widgetDescription["maxValue"].type()) continue;

                // minValue and maxValue type have to be equal here, only checking one
                if (widgetDescription["minValue"].type() != json::value_t::number_integer && widgetDescription["minValue"].type() != json::value_t::number_unsigned) continue;

                widget = new ui::widget::WidgetInteger(itemDescription["name"], widgetDescription["minValue"], widgetDescription["maxValue"]);
            }
            else if (widgetDescription["type"] == "bool") {
                Argument onValue, offValue;

                if (widgetDescription["onValue"].type() != widgetDescription["offValue"].type()) continue;

                // onValue and offValue type have to be equal here, only checking one

                onValue = jsonToArg(widgetDescription["onValue"]);
                offValue = jsonToArg(widgetDescription["offValue"]);

                widget = new ui::widget::WidgetBoolean(itemDescription["name"], onValue, offValue);

            }
            else if (widgetDescription["type"] == "string") {
                if (widgetDescription["minLength"].type() != widgetDescription["maxLength"].type()) continue;

                // minValue and maxValue type have to be equal here, only checking one
                if (widgetDescription["minLength"].type() != json::value_t::number_integer && widgetDescription["minLength"].type() != json::value_t::number_unsigned) continue;

                widget = new ui::widget::WidgetString(itemDescription["name"], widgetDescription["minLength"], widgetDescription["maxLength"]);
            }
            else if (widgetDescription["type"] == "list") {
                if (widgetDescription["keys"].type() != json::value_t::array || widgetDescription["values"].type() != json::value_t::array) continue;
                if (widgetDescription["keys"].size() != widgetDescription["values"].size()) continue;
                
                std::vector<NamedArgument> entries;

                for (u16 i = 0; i < widgetDescription["keys"].size(); i++)
                    entries.push_back(jsonToArg(widgetDescription["keys"][i], widgetDescription["values"][i]));
                
                widget = new ui::widget::WidgetList(itemDescription["name"], entries);
            }
            else if (widgetDescription["type"] == "slider") {

            }
            else if (widgetDescription["type"] == "progress") {
                widget = new ui::widget::WidgetProgressbar(itemDescription["name"]);
            }
            else if (widgetDescription["type"] == "comment") {
                if (widgetDescription["comment"].type() != json::value_t::string) continue;

                widget = new ui::widget::WidgetComment(itemDescription["name"], widgetDescription["comment"]);
            }


            if (widget != nullptr) {
                if (jsonExists(itemDescription, "infoText"))
                    widget->setDescription(itemDescription["infoText"]);
                    
                for (auto &[name, value] : widgetDescription["arguments"].items()) {
                    Argument argument;

                    if (value.type() == json::value_t::number_integer || value.type() == json::value_t::number_unsigned)
                        argument = value.get<s128>();
                    else if (value.type() == json::value_t::number_float)
                        argument = value.get<double>();
                    else if (value.type() == json::value_t::boolean)
                        argument = value.get<bool>();
                    else if (value.type() == json::value_t::string)
                        argument =  value.get<std::string>();
                    else continue;

                    widget->addArgument(name, argument);
                }  
                
                this->m_fileConfigs[fileNum].categories[itemDescription["category"]].widgets.push_back(widget);
            }
            
        }
    }

    void Config::createUI(brls::TabFrame *tabFrame) {
        if (tabFrame == nullptr) return;

        for (auto &[fileNum, fileConfig] : this->m_fileConfigs) {
            for (auto &[categoryName, category] : fileConfig.categories) {
                brls::List *list = new brls::List();

                for (auto &widget : category.widgets)
                    list->addView(widget->getView());

                tabFrame->addTab(categoryName, list);
            }
        }
    }

    std::string Config::getAuthor() {
        return this->m_author;
    }

    std::string Config::getDescription() {
        return this->m_description;
    }

    ScriptLanguage Config::getScriptLanguage() {
        return this->m_scriptLanguage;
    }

    std::string Config::getScript() {
        return this->m_script;
    }

    bool Config::isBeta() {
        return this->m_isBeta;
    }

    std::vector<std::string> Config::getSaveFilePaths() {
        std::vector<std::string> result;

        for (auto &[num, fileConfig] : this->m_fileConfigs)
            for (auto &path : fileConfig.saveFileNames)
                result.push_back(path);

        return result;
    }

    void Config::setScript(save::edit::Script *script) {
        for (auto &[num, fileConfig] : this->m_fileConfigs)
            for (auto &[name, category] : fileConfig.categories)
                for (auto &widget : category.widgets)
                    widget->setScript(script);
    }

}
