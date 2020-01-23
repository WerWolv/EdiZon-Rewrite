/**
 * Copyright (C) 2020 WerWolv
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

#include "helpers/file.hpp"

#include "save/title.hpp"
#include "save/account.hpp"

#include "save/edit/config.hpp"
#include "save/edit/script/script.hpp"

namespace edz::save::edit {

    class Editor {
    public:
        Editor(Editor const&) = delete;
        void operator=(Editor const&) = delete;

        static std::pair<EResult, Config*> loadConfig(std::unique_ptr<Title> &title, std::unique_ptr<Account> &account);
        static std::pair<EResult, Script*> loadScript(std::unique_ptr<Title> &title, std::unique_ptr<Account> &account, std::string saveFilePath);
        static EResult store();
        static void cleanUp();

    private:
        Editor();
        ~Editor();

        static inline Config *s_config = nullptr;
        static inline Script *s_script = nullptr;
        static inline edz::hlp::File *s_currentSaveFile = nullptr;
    };

}
