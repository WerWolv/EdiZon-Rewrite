#pragma once

#include "edizon.hpp"

#include "save/title.hpp"
#include "save/edit/widgets/widget.hpp"

#include <string>
#include <vector>
#include <map>

namespace nlohmann { class json; }

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

        std::map<std::string, widget::Widget*> m_widgets;


        void parseMetadata(nlohmann::json &j);
        void parseVersionSpecificMetadata(nlohmann::json &j);
        void parseWidgets(nlohmann::json &j);
    };

}