/**
 * Copyright (C) 2019 - 2020 WerWolv
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

    class AnonfileAPI {
    public:
        AnonfileAPI();
        ~AnonfileAPI();

        EResultVal<std::string> upload(std::vector<u8> &buffer, std::string fileName);
        EResultVal<std::string> upload(std::string path);

    private:
        edz::hlp::Curl m_curl;

        EResultVal<std::string> getFileURL(std::string responseData);
    };

}