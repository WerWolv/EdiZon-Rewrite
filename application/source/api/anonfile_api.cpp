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

#include "api/anonfile_api.hpp"
#include <nlohmann/json.hpp>

namespace edz::api {

    using json = nlohmann::json;

    AnonfileAPI::AnonfileAPI() : m_curl(ANONFILE_API_URL) {

    }

    AnonfileAPI::~AnonfileAPI() {

    }


    EResultVal<std::string> AnonfileAPI::upload(std::vector<u8> &buffer, std::string fileName) {
        auto [result, response] = this->m_curl.upload("/upload", fileName, buffer);

        if (result.succeeded())
            return AnonfileAPI::getFileURL(response);

        return { ResultEdzAPIError, "" };
    }

    EResultVal<std::string> AnonfileAPI::upload(std::string path) {
        auto [result, response] =  this->m_curl.upload("/upload", path);

        if (result.succeeded())
            return AnonfileAPI::getFileURL(response);

        return { ResultEdzAPIError, "" };
    }

    EResultVal<std::string> AnonfileAPI::getFileURL(std::string responseData) {
        try {
            json responseJson = json::parse(responseData);

            if (!responseJson["status"])
                return { ResultEdzAPIError, responseJson["error"]["message"] };

            return { ResultSuccess, responseJson["data"]["file"]["url"]["short"] };
        } catch (std::exception& e) {
            return { ResultEdzAPIError, "" };
        }

        return { ResultEdzAPIError, "" };
    }

}