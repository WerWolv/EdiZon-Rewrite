#pragma once

#include "edizon.hpp"
#include <string>
#include <vector>
#include <map>

#include "save/edit/widgets/widget.hpp"

namespace edz::save::edit {

    class Script {
    public:
        Script(std::string scriptName);
        virtual ~Script();

        void setSaveFileData(u8 *buffer, size_t bufferSize);
        void addArgument(std::string argName, std::shared_ptr<widget::Arg> arg);
        void clearArguments();


        virtual std::shared_ptr<widget::Arg> getValue() = 0;
        virtual void setValue(std::shared_ptr<widget::Arg> value) = 0;
        virtual std::vector<u8> getModifiedSaveFileData() = 0;

    protected:
        std::string m_scriptName;
        std::map<std::string, std::shared_ptr<widget::Arg>> m_arguments;

        u8 *m_saveFileData;
        size_t m_saveFileSize;
    };

}