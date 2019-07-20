#include "save/edit/script/script.hpp"

#include <cstring>

namespace edz::save::edit {

    Script::Script(std::string scriptName) {
        this->m_saveFileData = nullptr;
    }

    Script::~Script() {
        if (this->m_saveFileData != nullptr)
            delete[] this->m_saveFileData;
    }

    void Script::setSaveFileData(u8 *buffer, size_t bufferSize) {
        if (this->m_saveFileData != nullptr)
            delete[] this->m_saveFileData;

        this->m_saveFileData = new u8[bufferSize];
        this->m_saveFileSize = bufferSize;

        std::memcpy(this->m_saveFileData, buffer, bufferSize);
    }

    void Script::addArgument(std::string argName, std::shared_ptr<widget::Arg> arg) {
        this->m_arguments.insert({ argName, arg });
    }

    void Script::clearArguments() {
        this->m_arguments.clear();
    }

}