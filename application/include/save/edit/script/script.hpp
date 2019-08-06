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

#pragma once

#include <edizon.hpp>

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