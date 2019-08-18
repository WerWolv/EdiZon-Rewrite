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
#include "helpers/curl.hpp"

#include <string>
#include <vector>

namespace edz::api {

    class SwitchCheatsDBAPI {
    public:
        SwitchCheatsDBAPI();
        ~SwitchCheatsDBAPI();

        typedef struct {
            u32 id;
            buildid_t buildID;
            std::string content;
            std::string credits;
        } cheat_t;

        typedef struct {
            u32 id;
            std::string name;
            std::string creationDate;
            std::string url;
            userid_t userID;
            std::string userName;
        } save_file_t;

        typedef struct {
            std::string slug;
            std::string name;
            std::string image;
            titleid_t titleID;
            std::vector<cheat_t> cheats;
        } cheat_response_t; 

        // Unauthenticated
        std::pair<EResult, std::string> getToken(std::string email, std::string password);

        // Authenticated
        std::pair<EResult, cheat_response_t> getCheats(titleid_t titleID, buildid_t buildID = 0);
        std::pair<EResult, u32> getCheatCount();
        std::pair<EResult, save_file_t> getSaveFiles();
        EResult addSaveFile();

    private:
        edz::hlp::Curl m_curl;
    };

}