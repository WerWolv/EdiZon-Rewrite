#include "config.hpp"

#include "save/save_data.hpp"

#include <nlohmann/json.hpp>
#include <fstream>
#include <regex>

#include "save/edit/widgets/widget_integer.hpp"
#include "save/edit/widgets/widget_boolean.hpp"
#include "save/edit/widgets/widget_string.hpp"
#include "save/edit/widgets/widget_list.hpp"
#include "save/edit/widgets/widget_slider.hpp"
#include "save/edit/widgets/widget_progressbar.hpp"
#include "save/edit/widgets/widget_comment.hpp"

using json = nlohmann::json;

#define CONFIG_PATH EDIZON_BASE_DIR"/configs/"

namespace edz::save::edit {

    Config::Config(Title *title, Account *account) : m_title(title), m_account(account) {

        try {

            std::ifstream configFile(CONFIG_PATH + title->getIDString() + ".json");

            if (!configFile.is_open()) {
                // File couldn't be opened, no config file present
                this->m_configLoadState = ConfigLoadState::NO_CONFIG;
                return;
            }
            
            // Parse config file

            json config;
            config << configFile;
            
            // Load basic config information
            parseMetadata(config);
            

            if (!jsonExists(config, title->getVersionString()) && !jsonExists(config, "all")) {
                // No save editor for this game version configured
                this->m_configLoadState = ConfigLoadState::CONFIG_OUTDATED;
                return;
            }

            json configVersionMetadata;

            if (jsonExists(config, title->getVersionString()))
                configVersionMetadata = config[title->getVersionString()];
            else configVersionMetadata = config["all"];

            parseVersionSpecificMetadata(configVersionMetadata);

            if (jsonExists(configVersionMetadata, "items"))
                parseWidgets(configVersionMetadata["items"]);
            else throw json::parse_error::create(100, 0, "No widgets specified");

        } catch (json::parse_error &e) {
            this->m_configLoadState = ConfigLoadState::SYNTAX_ERROR;
            return;
        }

        this->m_configLoadState = ConfigLoadState::LOADED;
    }

    Config::~Config() {
        
    }


    ConfigLoadState Config::getLoadState() {
        return this->m_configLoadState;
    }
    

    bool jsonExists(json &j, std::string key) {
        return j.find(key) != j.end();
    }

    widget::Arg jsonToArg(json::value_type jsonValue) {
        widget::Arg ret = nullptr;

        if (jsonValue.type() == json::value_t::number_integer || jsonValue.type() == json::value_t::number_unsigned)
            ret = widget::_Arg::create(jsonValue.get<s64>());
        else if (jsonValue.type() == json::value_t::number_float)
            ret = widget::_Arg::create(jsonValue.get<double>());
        else if (jsonValue.type() == json::value_t::boolean)
            ret = widget::_Arg::create(jsonValue.get<bool>());
        else if (jsonValue.type() == json::value_t::string)
            ret = widget::_Arg::create(jsonValue.get<std::string>());

        return ret;
    }


    void Config::parseMetadata(nlohmann::json &j) {
        this->m_author = j["author"];
        this->m_description = j["description"];
        this->m_isBeta = j["beta"];

        if (j["scriptLanguage"] == "lua") 
            this->m_scriptLanguage = ScriptLanguage::LUA;
        else if (j["scriptLanguage"] == "py" || j["scriptLanguage"] == "python")
            this->m_scriptLanguage = ScriptLanguage::PYTHON;
    }

    void Config::parseVersionSpecificMetadata(nlohmann::json &j) {
        auto saveFileSystem = new SaveFileSystem(this->m_title, this->m_account);
        std::vector<std::string> pathRegex = j["pathRegex"];
        std::string fileNameRegex = j["fileNameRegex"];

        std::vector<std::string> matchingPaths;

        bool inRootDirectory = true;
        u8 folderDepth = 0;

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
                    saveFileSystem->getSaveFolder().getFolders()[path].foreach([&](struct dirent* entry){
                        if (entry->d_type == DT_DIR) {
                            if (std::regex_match(std::string(entry->d_name), std::regex(folderRegex))) {
                                if (folderDepth < (pathRegex.size() - 1))
                                    tempMatchingPaths.push_back(path + "/" + entry->d_name);
                                else // We reached the last level. Store the full paths for loading later
                                    this->m_saveFilePaths.push_back(path + "/" + entry->d_name);
                            }
                        }
                    });
                }

            }

            folderDepth++;
            matchingPaths = tempMatchingPaths;
        }

        
        for (std::string path : this->m_saveFilePaths) {
            // Save data is still mounted to save:/ 
            edz::helper::Folder saveFileFolder("save:/" + path);

            for (auto &[fileName, file] : saveFileFolder.getFiles())
                if (std::regex_match(fileName, std::regex(fileNameRegex)))
                    this->m_saveFileNames.push_back(fileName);
        }

    }

    void Config::parseWidgets(nlohmann::json &j) {
        for (auto itemDescription : j) {
            auto widgetDescription = j["widget"];

            widget::Widget *widget = nullptr;

            if (widgetDescription["type"] == "int") {
                if (widgetDescription["minValue"].type() != widgetDescription["maxValue"].type()) continue;

                // minValue and maxValue type have to be equal here, only checking one
                if (widgetDescription["minValue"].type() != json::value_t::number_integer && widgetDescription["minValue"].type() != json::value_t::number_unsigned) continue;

                widget = new widget::WidgetInteger(itemDescription["name"], widgetDescription["minValue"], widgetDescription["maxValue"]);
            }
            else if (widgetDescription["type"] == "bool") {
                widget::Arg onValue, offValue;

                if (widgetDescription["onValue"].type() != widgetDescription["offValue"].type()) continue;

                // onValue and offValue type have to be equal here, only checking one

                onValue = jsonToArg(widgetDescription["onValue"]);
                offValue = jsonToArg(widgetDescription["offValue"]);

            }
            else if (widgetDescription["type"] == "string") {
                if (widgetDescription["minLength"].type() != widgetDescription["maxLength"].type()) continue;

                // minValue and maxValue type have to be equal here, only checking one
                if (widgetDescription["minLength"].type() != json::value_t::number_integer && widgetDescription["minLength"].type() != json::value_t::number_unsigned) continue;

                widget = new widget::WidgetString(itemDescription["name"], widgetDescription["minLength"], widgetDescription["minLength"]);
            }
            else if (widgetDescription["type"] == "list") {
                if (widgetDescription["keys"].type() != json::value_t::array || widgetDescription["values"].type() != json::value_t::array) continue;
                
                std::vector<std::string> keys = widgetDescription["keys"];
                std::vector<widget::Arg> values;

                for (auto value : widgetDescription["values"])
                    values.push_back(jsonToArg(value));
                
                widget = new widget::WidgetList(itemDescription["name"], keys, values);
            }
            else if (widgetDescription["type"] == "slider") {

            }
            else if (widgetDescription["type"] == "progress") {

            }
            else if (widgetDescription["type"] == "comment") {
                if (widgetDescription["comment"].type() != json::value_t::string) continue;

                widget = new widget::WidgetComment(itemDescription["name"], widgetDescription["comment"]);
            }
            
            if (widget != nullptr)
                this->m_widgets.insert({ itemDescription["category"], widget });
        }
    }

}