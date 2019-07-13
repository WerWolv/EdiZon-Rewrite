#pragma once

#include "edizon.hpp"

#include "save/title.hpp"
#include "save/edit/widgets/widget.hpp"

#include <string>
#include <vector>
#include <map>

#include <nlohmann/json_fwd.hpp>

namespace edz::save::edit {

    enum class ScriptLanguage {
        LUA,
        PYTHON
    };

    enum class ConfigLoadState {
        LOADED,
        NO_CONFIG,
        SYNTAX_ERROR,
        CONFIG_OUTDATED,
        TOO_MANY_REDIRECTS
    };

    typedef struct {
        std::string name;
        std::vector<widget::Widget*> widgets;
    } WidgetCategory;

    typedef struct {
        std::vector<std::string> saveFilePaths;
        std::vector<std::string> saveFileNames;

        std::vector<WidgetCategory> categories;
    } FileConfig;

    class Config {
    public:
        Config(Title *title, Account *account);
        ~Config();

        void createUI(TabFrame *tabFrame);
        ConfigLoadState getLoadState();
    private:
        Title *m_title;
        Account *m_account;

        std::string m_author;
        ScriptLanguage m_scriptLanguage;
        bool m_isBeta;
        std::string m_description;

        ConfigLoadState m_configLoadState;

        std::map<u16, FileConfig> m_fileConfigs;

        bool jsonExists(nlohmann::json &j, std::string key);

        void parseMetadata(nlohmann::json &j);
        void parseVersionSpecificMetadata(nlohmann::json &j, u16 fileNum);
        void parseWidgets(nlohmann::json &j, u16 fileNum);
    };

}