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

namespace edz::web {

    class EdiZonAPI {
    public:
        EdiZonAPI();
        ~EdiZonAPI();

        typedef struct {
            std::string name;
            std::string owner;
            std::string description;
            std::string url;
        } official_provider_t;

        typedef struct {
            std::string name;
            std::string changelog;
            std::string tag;
            std::string date;
            u32 downloadCount;
        } release_info_t;

        std::vector<official_provider_t> getOfficialProviders();
        std::vector<release_info_t> getReleaseInfo();

    private:
        edz::helper::Curl curl;
    };

}