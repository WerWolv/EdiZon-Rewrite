#pragma once

#include "edizon.hpp"

#include "save/title.hpp"
#include "save/edit/widgets/widget.hpp"

#include <string>
#include <vector>
#include <map>

namespace edz::save::edit {

    enum class ScriptLanguage {
        LUA,
        PYTHON
    };

    class Config {
    public:
        Config(Title *title);
        ~Config();

        void createUI(TabFrame *tabFrame);
        bool exists();
    private:
        std::string m_author;
        ScriptLanguage m_scriptLanguage;
        bool m_isBeta;
        std::string m_description;

        std::vector<std::string> m_pathRegex;
        std::string m_fileNameRegex;

        std::map<std::string, widget::Widget*> m_widgets;
    };

}