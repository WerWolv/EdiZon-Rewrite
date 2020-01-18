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

    SaveRepoAPI::SaveRepoAPI(std::string apiBase) : m_curl(apiBase), m_version("") {

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

    std::pair<EResult, std::string> SaveRepoAPI::getVersion() {
        std::string version;

        auto [result, response] = this->m_curl.get("/version");

        if (result.failed())
            return { ResultEdzAPIError, EMPTY_RESPONSE };

        try {
            json responseJson = json::parse(response);

            version = responseJson["version"];
        } catch (std::exception& e) {
            return { ResultEdzAPIError, EMPTY_RESPONSE };
        }

        return { ResultSuccess, version };
    }

    std::pair<EResult, std::vector<u8>> SaveRepoAPI::getIcon() {
        SaveRepoAPI::_updateVersionString();

        return this->m_curl.download("/" + this->m_version + "/icon");
    }

    EResult SaveRepoAPI::getFile(std::string file, std::string downloadPath) {
        SaveRepoAPI::_updateVersionString();

        return this->m_curl.download("/" + this->m_version + "/get?fileName=" + file, downloadPath);
    }

    std::pair<EResult, std::vector<SaveRepoAPI::save_file_t>> SaveRepoAPI::listFiles() {
        std::vector<SaveRepoAPI::save_file_t> saveFiles;
        SaveRepoAPI::_updateVersionString();

        auto [result, response] = this->m_curl.get("/" + this->m_version + "/list");

        if (result.failed())
            return { ResultEdzAPIError, EMPTY_RESPONSE };

        try {
            json responseJson = json::parse(response);

            for (auto provider : responseJson["saveFiles"])
                saveFiles.push_back({ provider["fileName"], provider["date"], strtoull(provider["titleID"].get<std::string>().c_str(), nullptr, 16) });
        } catch (std::exception& e) {
            return { ResultEdzAPIError, EMPTY_RESPONSE };
        }

        return { ResultSuccess, saveFiles };
    }


    void SaveRepoAPI::_updateVersionString() {
        if (!this->m_version.empty())
            return;

        auto [result, version] = SaveRepoAPI::getVersion();
        if (result.succeeded())
            this->m_version = version;
    }

}