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

#include "api/switchcheatsdb_api.hpp"
#include <nlohmann/json.hpp>
#include "helpers/utils.hpp"

namespace edz::api {

    using json = nlohmann::json;

    SwitchCheatsDBAPI::SwitchCheatsDBAPI() : m_curl(SWITCHCHEATSDB_API_URL) {

    }

    SwitchCheatsDBAPI::~SwitchCheatsDBAPI() {

    }


    std::pair<EResult, std::string> SwitchCheatsDBAPI::getToken(std::string email, std::string password) {
        std::string token;

        json body;
        body["email"] = email;
        body["password"] = password;

        auto [result, response] = this->m_curl.post("/token", body.dump());

        if (result.failed())
            return { ResultEdzAPIError, "" };

        try {
            json responseJson = json::parse(response);
            token = responseJson["token"];
        } catch (std::exception& e) {
            printf("%s\n", response.c_str());
            return { ResultEdzAPIError, "" };
        }

        return { ResultSuccess, token };
    }


    std::pair<EResult, SwitchCheatsDBAPI::cheat_response_t> SwitchCheatsDBAPI::getCheats(titleid_t titleID, buildid_t buildID) {

    }

    std::pair<EResult, u32> SwitchCheatsDBAPI::getCheatCount() {

    }

    std::pair<EResult, SwitchCheatsDBAPI::save_file_t> SwitchCheatsDBAPI::getSaveFiles() {

    }

    EResult SwitchCheatsDBAPI::addSaveFile() {

    }

}