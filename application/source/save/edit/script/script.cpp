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

#include "save/edit/script/script.hpp"

#include <cstring>

namespace edz::save::edit {

    Script::Script(std::string path) {
        this->m_saveData = nullptr;
    }

    Script::~Script() {
        if (this->m_saveData != nullptr)
            delete[] this->m_saveData;
    }

    void Script::setSaveData(u8 *buffer, size_t bufferSize) {
        if (this->m_saveData != nullptr)
            delete[] this->m_saveData;

        this->m_saveData = new u8[bufferSize];
        this->m_saveSize = bufferSize;

        std::memcpy(this->m_saveData, buffer, bufferSize);
    }

    void Script::addArgument(std::string argName, Argument arg) {
        this->m_arguments.insert({ argName, arg });
    }

    void Script::clearArguments() {
        this->m_arguments.clear();
    }

}