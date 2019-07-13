#include "save/edit/config.hpp"

#include "save/save_data.hpp"


#include <fstream>
#include <regex>

#include "save/edit/widgets/widget_integer.hpp"
#include "save/edit/widgets/widget_boolean.hpp"
#include "save/edit/widgets/widget_string.hpp"
#include "save/edit/widgets/widget_list.hpp"
#include "save/edit/widgets/widget_slider.hpp"
#include "save/edit/widgets/widget_progressbar.hpp"
#include "save/edit/widgets/widget_comment.hpp"

#include <nlohmann/json.hpp>

#define CONFIG_PATH EDIZON_BASE_DIR"/configs/"

using json = nlohmann::json;

namespace edz::save::edit {

    std::shared_ptr<widget::Arg> jsonToArg(json::value_type jsonValue) {
        std::shared_ptr<widget::Arg> ret = nullptr;

        if (jsonValue.type() == json::value_t::number_integer || jsonValue.type() == json::value_t::number_unsigned)
            ret = widget::Arg::create(jsonValue.get<s64>());
        else if (jsonValue.type() == json::value_t::number_float)
            ret = widget::Arg::create(jsonValue.get<double>());
        else if (jsonValue.type() == json::value_t::boolean)
            ret = widget::Arg::create(jsonValue.get<bool>());
        else if (jsonValue.type() == json::value_t::string)
            ret = widget::Arg::create(jsonValue.get<std::string>());

        return ret;
    }
    
    Config::Config(Title *title, Account *account) : m_title(title), m_account(account) {

        try {
            std::ifstream configFile(CONFIG_PATH"test.json");
            if (!configFile.is_open()) {
                // File couldn't be opened, no config file present
                this->m_configLoadState = ConfigLoadState::NO_CONFIG;
                return;
            }

            // Parse config file
            json config;
            configFile >> config;

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

            u16 fileNum = 0;
            for (auto &fileDefinitions : configVersionMetadata) {
                parseVersionSpecificMetadata(fileDefinitions, fileNum);

                if (jsonExists(fileDefinitions, "items"))
                    parseWidgets(fileDefinitions["items"], fileNum);
                else throw json::parse_error::create(100, 0, "No widgets specified");

                fileNum++;
            }

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
    

    bool Config::jsonExists(json &j, std::string key) {
        return j.find(key) != j.end();
    }


    void Config::parseMetadata(json &j) {
        this->m_author = j["author"];
        this->m_description = j["description"];
        this->m_isBeta = j["beta"];

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
            // Save data is still mounted to save:/ 
            edz::helper::Folder saveFileFolder("save:/" + path);

            for (auto &[fileName, file] : saveFileFolder.getFiles())
                if (std::regex_match(fileName, std::regex(fileNameRegex)))
                    fileConfig.saveFileNames.push_back(fileName);
        }

        m_fileConfigs[fileNum] = fileConfig;
    }

    void Config::parseWidgets(json &j, u16 fileNum) {
        for (auto itemDescription : j) {
            auto widgetDescription = itemDescription["widget"];
            DEBUG_PRINT();
            widget::Widget *widget = nullptr;
DEBUG_PRINT();
            if (widgetDescription["type"] == "int") {
                if (widgetDescription["minValue"].type() != widgetDescription["maxValue"].type()) continue;
DEBUG_PRINT();
                // minValue and maxValue type have to be equal here, only checking one
                if (widgetDescription["minValue"].type() != json::value_t::number_integer && widgetDescription["minValue"].type() != json::value_t::number_unsigned) continue;

                widget = new widget::WidgetInteger(itemDescription["name"], widgetDescription["minValue"], widgetDescription["maxValue"]);
            }
            else if (widgetDescription["type"] == "bool") {
                std::shared_ptr<widget::Arg> onValue, offValue;

                if (widgetDescription["onValue"].type() != widgetDescription["offValue"].type()) continue;

                // onValue and offValue type have to be equal here, only checking one

                onValue = jsonToArg(widgetDescription["onValue"]);
                offValue = jsonToArg(widgetDescription["offValue"]);

                widget = new widget::WidgetBoolean(itemDescription["name"], onValue, offValue);

            }
            else if (widgetDescription["type"] == "string") {
                if (widgetDescription["minLength"].type() != widgetDescription["maxLength"].type()) continue;

                // minValue and maxValue type have to be equal here, only checking one
                if (widgetDescription["minLength"].type() != json::value_t::number_integer && widgetDescription["minLength"].type() != json::value_t::number_unsigned) continue;

                widget = new widget::WidgetString(itemDescription["name"], widgetDescription["minLength"], widgetDescription["maxLength"]);
            }
            else if (widgetDescription["type"] == "list") {
                /*if (widgetDescription["keys"].type() != json::value_t::array || widgetDescription["values"].type() != json::value_t::array) continue;
                
                std::vector<std::string> keys = widgetDescription["keys"];
                std::vector<std::shared_ptr<widget::Arg>> values;

                for (auto value : widgetDescription["values"])
                    values.push_back(jsonToArg(value));
                
                widget = new widget::WidgetList(itemDescription["name"], keys, values);*/
            }
            else if (widgetDescription["type"] == "slider") {

            }
            else if (widgetDescription["type"] == "progress") {

            }
            else if (widgetDescription["type"] == "comment") {
                /*if (widgetDescription["comment"].type() != json::value_t::string) continue;

                widget = new widget::WidgetComment(itemDescription["name"], widgetDescription["comment"]);*/
            }
            DEBUG_PRINT();

            if (widget != nullptr) {
                DEBUG_PRINT();
                if (jsonExists(itemDescription, "infoText"))
                    widget->setDescription(itemDescription["infoText"]);
DEBUG_PRINT();
                for (auto &[name, value] : widgetDescription["arguments"].items()) {
                    std::shared_ptr<widget::Arg> argument;

                    if (value.type() == json::value_t::number_integer || value.type() == json::value_t::number_unsigned)
                        argument = widget::Arg::create(value.get<s128>());
                    else if (value.type() == json::value_t::number_float)
                        argument = widget::Arg::create(value.get<double>());
                    else if (value.type() == json::value_t::boolean)
                        argument = widget::Arg::create(value.get<bool>());
                    else if (value.type() == json::value_t::string)
                        argument = widget::Arg::create(value.get<std::string>());
                    else continue;

                    widget->addArgument(name, argument);
                }  
                
                this->m_fileConfigs[fileNum].categories[itemDescription["category"]].widgets.push_back(widget);
            }
            
        }
    }

    void Config::createUI(TabFrame *tabFrame) {
        if (tabFrame == nullptr) return;

        for (auto &[fileNum, fileConfig] : this->m_fileConfigs) {
            for (auto &[categoryName, category] : fileConfig.categories) {
                List *list = new List();

                for (auto &widget : category.widgets)
                    list->addView(widget->getView());

                tabFrame->addTab(categoryName, list);
            }
        }
    }

}
