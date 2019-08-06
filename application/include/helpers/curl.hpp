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

#include <string>
#include <vector>
#include <functional>
#include <curl/curl.h>

namespace edz::helper {

    class Curl {
    public:
        Curl(std::string baseURL);
        ~Curl();

        EResult get(std::string path, std::string &response);

        EResult download(std::string path, std::vector<u8> &data, std::string &response);
        EResult download(std::string path, std::string downloadPath, std::string &response);

        EResult upload(std::string path, std::string fileName, std::vector<u8> &data, std::string &response);
        EResult upload(std::string path, std::vector<u8> uploadPath, std::string &response);

        std::function<bool(u8)>& getProgressCallback();
        void setProgressCallback(std::function<bool(u8)> callback);

    private:
        std::string m_baseURL;
        CURL *m_curl;
        std::function<bool(u8)> m_progressCallback;
    };

}