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

#include "api/save_repo_api.hpp"
#include <nlohmann/json.hpp>

namespace edz::api {

    using json = nlohmann::json;

    SaveRepoAPI::SaveRepoAPI(std::string apiBase) : m_curl(apiBase) {

    }

    SaveRepoAPI::~SaveRepoAPI() {

    }


    std::pair<EResult, std::string> SaveRepoAPI::getName() {
        std::string name;

        auto [result, response] = this->m_curl.get("/name");

        if (result.failed())
            return { ResultEdzAPIError, EMPTY_RESPONSE };

        try {
            json responseJson = json::parse(response);

            name = responseJson["name"];
        } catch (std::exception& e) {
            return { ResultEdzAPIError, EMPTY_RESPONSE };
        }

        return { ResultSuccess, name };
    }

    std::pair<EResult, std::string> SaveRepoAPI::getMOTD() {
        std::string name;

        auto [result, response] = this->m_curl.get("/motd");

        if (result.failed())
            return { ResultEdzAPIError, EMPTY_RESPONSE };

        try {
            json responseJson = json::parse(response);

            name = responseJson["motd"];
        } catch (std::exception& e) {
            return { ResultEdzAPIError, EMPTY_RESPONSE };
        }

        return { ResultSuccess, name };
    }

    std::pair<EResult, std::vector<u8>> SaveRepoAPI::getIcon() {
        return this->m_curl.download("/icon");
    }

    std::pair<EResult, std::vector<u8>> SaveRepoAPI::getFile(std::string file) {
        return this->m_curl.download("/get?fileName=" + file);
    }

    std::pair<EResult, std::vector<SaveRepoAPI::save_file_t>> SaveRepoAPI::listFiles() {
        std::vector<SaveRepoAPI::save_file_t> saveFiles;

        auto [result, response] = this->m_curl.get("/list");

        if (result.failed())
            return { ResultEdzAPIError, EMPTY_RESPONSE };

        try {
            json responseJson = json::parse(response);

            for (auto provider : responseJson["save_files"])
                saveFiles.push_back({ provider["name"], provider["date"] });
        } catch (std::exception& e) {
            return { ResultEdzAPIError, EMPTY_RESPONSE };
        }

        return { ResultSuccess, saveFiles };
    }

}