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
#include <memory>

#include "ui/widgets/widget.hpp"

using edz::ui::widget::Argument;

namespace edz::save::edit {

    class Script {
    public:
        Script(std::string path);
        virtual ~Script();

        virtual void setSaveData(u8 *buffer, size_t bufferSize) final;
        virtual void addArgument(std::string argName, Argument arg) final;
        virtual void clearArguments() final;


        virtual std::tuple<EResult, Argument> getValue() = 0;
        virtual EResult setValue(Argument value) = 0;
        virtual std::tuple<EResult, std::optional<std::vector<u8>>> getModifiedSaveData() = 0;

    protected:
        std::string m_scriptName;
        std::map<std::string, Argument> m_arguments;

        u8 *m_saveData;
        size_t m_saveSize;
    };

}