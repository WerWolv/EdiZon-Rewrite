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

#include "api/edizon_api.hpp"
#include <nlohmann/json.hpp>

namespace edz::api {

    using json = nlohmann::json;

    EdiZonAPI::EdiZonAPI() : m_curl(EDIZON_API_URL) {

    }

    EdiZonAPI::~EdiZonAPI() {

    }


    std::pair<EResult, std::vector<EdiZonAPI::official_provider_t>> EdiZonAPI::getOfficialProviders() {
        std::vector<EdiZonAPI::official_provider_t> providers;

        auto [result, response] = this->m_curl.get("/v4/official_providers");

        if (result.failed())
            return { ResultEdzAPIError, EMPTY_RESPONSE };

        try {
            json responseJson = json::parse(response);

            for (auto provider : responseJson["providers"])
                providers.push_back({ provider["name"], provider["owner"], provider["description"], provider["url"] });
        } catch (std::exception& e) {
            return { ResultEdzAPIError, EMPTY_RESPONSE };
        }

        return { ResultSuccess, providers };
    }

    std::pair<EResult, EdiZonAPI::release_info_t> EdiZonAPI::getReleaseInfo() {
        EdiZonAPI::release_info_t releaseInfo;

        auto [result, response] = this->m_curl.get("/v4/release");

        if (result.failed())
            return { ResultEdzAPIError, EMPTY_RESPONSE };

        try {
            json responseJson = json::parse(response);

            releaseInfo = { responseJson["name"], responseJson["changelog"], responseJson["tag"], responseJson["date"], responseJson["download_count"] };
        } catch (std::exception& e) {
            return { ResultEdzAPIError, EMPTY_RESPONSE };
        }

        return { ResultSuccess, releaseInfo };
    }

    std::pair<EResult, std::vector<EdiZonAPI::notification_t>> EdiZonAPI::getNotifications() {
        std::vector<EdiZonAPI::notification_t> notifications;

        auto [result, response] = this->m_curl.get("/v4/notifications");

        if (result.failed())
            return { ResultEdzAPIError, EMPTY_RESPONSE };

        try {
            json responseJson = json::parse(response);

            for (auto provider : responseJson["notifications"])
                notifications.push_back({ provider["title"], provider["description"], provider["icon"] });
        } catch (std::exception& e) {
            return { ResultEdzAPIError, EMPTY_RESPONSE };
        }

        return { ResultSuccess, notifications };
    }

}