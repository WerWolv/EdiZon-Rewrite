#pragma once

#include "edizon.hpp"

#include <nlohmann/json.hpp>

#include "save/title.hpp"
#include "save/edit/widgets/widget.hpp"

#include <string>
#include <vector>
#include <map>

using json = nlohmann::json;

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

        std::vector<std::string> m_saveFilePaths;
        std::vector<std::string> m_saveFileNames;

        ConfigLoadState m_configLoadState;

        std::map<std::string, std::vector<widget::Widget*>> m_widgets;


        bool jsonExists(json &j, std::string key);
        std::shared_ptr<widget::Arg> jsonToArg(json::value_type jsonValue);


        void parseMetadata(json &j);
        void parseVersionSpecificMetadata(json &j);
        void parseWidgets(json &j);
    };

}