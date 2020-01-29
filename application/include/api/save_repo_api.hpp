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
#include "helpers/curl.hpp"

#include <string>
#include <vector>

namespace edz::api {

    class SaveRepoAPI {
    public:
        SaveRepoAPI(std::string apiBase);
        ~SaveRepoAPI();

        typedef struct {
            std::string name;
            std::string date;
            titleid_t titleID;
        } save_file_t;

        EResultVal<std::string> getName();
        EResultVal<std::string> getMOTD();
        EResultVal<std::string> getVersion();
        EResultVal<std::vector<u8>> getIcon();
        EResult getFile(std::string file, std::string downloadPath);
        EResultVal<std::vector<save_file_t>> listFiles();

        void _updateVersionString();

    private:
        edz::hlp::Curl m_curl;
        std::string m_version;
    };

}