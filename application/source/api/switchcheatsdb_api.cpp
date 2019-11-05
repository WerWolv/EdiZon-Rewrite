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
#include "helpers/config_manager.hpp"

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
            return { ResultEdzAPIError, "" };
        }

        return { ResultSuccess, token };
    }


    std::pair<EResult, SwitchCheatsDBAPI::cheat_response_t> SwitchCheatsDBAPI::getCheats(titleid_t titleID, buildid_t buildID) {
        SwitchCheatsDBAPI::cheat_response_t cheatResponse;
        Logger::debug("%s", ("/cheats/" + hlp::toHexString(titleID) + (buildID == 0 ? "" : "/" + hlp::toHexString(buildID))).c_str());
        auto [result, response] = this->m_curl.get("/cheats/" + hlp::toHexString(titleID) + (buildID == 0 ? "" : "/" + hlp::toHexString(buildID)), { {"X-API-TOKEN", GET_CONFIG(Online.switchcheatsdbApiToken)} });
        Logger::debug("%s", result.getString().c_str());

        if (result.failed())
            return { ResultEdzAPIError, EMPTY_RESPONSE };
        Logger::debug("1");
        try {
            json responseJson = json::parse(response);
            
            cheatResponse.slug    = responseJson["slug"];
            cheatResponse.name    = responseJson["name"];
            cheatResponse.image   = responseJson["image"];
            cheatResponse.titleID = strtol(responseJson["titleid"].get<std::string>().c_str(), nullptr, 16);

            u32 id = 0;
            for (auto cheat : responseJson["cheats"]) {
                //                                                 Yes this is a typo --------V
                cheatResponse.cheats.push_back({ id++, static_cast<buildid_t>(strtol(cheat["builid"].get<std::string>().c_str(), nullptr, 16)), cheat["content"], cheat["credits"] == nullptr ? "" : cheat["credits"].get<std::string>() });
            }

        } catch (std::exception& e) {
            Logger::debug("%s", e.what());
            return { ResultEdzAPIError, EMPTY_RESPONSE };
        }

        return { ResultSuccess, cheatResponse };
    }

    std::pair<EResult, u32> SwitchCheatsDBAPI::getCheatCount() {
        u32 count;

        auto [result, response] = this->m_curl.get("/cheats/count", { {"X-API-TOKEN", GET_CONFIG(Online.switchcheatsdbApiToken)} });

        if (result.failed())
            return { ResultEdzAPIError, 0 };

        try {
            json responseJson = json::parse(response);
            count = responseJson["count"];
        } catch (std::exception& e) {
            return { ResultEdzAPIError, 0 };
        }

        return { ResultSuccess, count };
    }

    std::pair<EResult, std::vector<SwitchCheatsDBAPI::save_file_t>> SwitchCheatsDBAPI::getSaveFiles() {
        std::vector<SwitchCheatsDBAPI::save_file_t> saveFiles;

        auto [result, response] = this->m_curl.get("/saves", { {"X-API-TOKEN", GET_CONFIG(Online.switchcheatsdbApiToken)} });

        if (result.failed())
            return { ResultEdzAPIError, EMPTY_RESPONSE };

        try {
            json responseJson = json::parse(response);
            for (auto &game : responseJson["games"]) {
                saveFiles.push_back({ "", game["created_at"], game["path"], 0, 0 });
            }
        } catch (std::exception& e) {
            return { ResultEdzAPIError, EMPTY_RESPONSE };
        }

        return { ResultSuccess, saveFiles };
    }

    EResult SwitchCheatsDBAPI::addSaveFile(std::string backupName, std::string link, std::unique_ptr<save::Title> &title) {
        json body;
        body["name"] = backupName;
        body["path"] = link;
        body["title_id"] = title->getIDString();

        auto [result, response] = this->m_curl.post("/saves", body.dump());

        if (result.failed())
            return ResultEdzAPIError;

        try {
            json responseJson = json::parse(response);
            if (responseJson["status"] != "ok")
                return ResultEdzAPIError;
        } catch (std::exception& e) {
            return ResultEdzAPIError;
        }

        return ResultSuccess;
    }

}