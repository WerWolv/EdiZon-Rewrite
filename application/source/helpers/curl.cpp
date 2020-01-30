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

#include "helpers/curl.hpp"
#include "helpers/file.hpp"

#include <cstring>

namespace edz::hlp {

    Curl::Curl(std::string baseURL) : m_baseURL(baseURL) {
        this->m_curl = curl_easy_init();

        this->m_progressCallback = [](u8 x){ return false; };
    }

    Curl::~Curl() {
        curl_easy_cleanup(this->m_curl);
    }

    static size_t writeToString(void *contents, size_t size, size_t nmemb, void *userp){
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    static size_t writeToFile(void *ptr, size_t size, size_t nmemb, FILE *stream) {
        size_t written = fwrite(ptr, size, nmemb, stream);
        return written;
    }

    static size_t writeToBuffer(void *contents, size_t size, size_t nmemb, void *userp) {
        size_t newDataSize = size * nmemb;
        std::vector<u8> *mem = (std::vector<u8>*)userp;
        size_t currDataSize = mem->size();
        mem->resize(currDataSize + newDataSize);
        std::memcpy(&(*mem)[currDataSize], contents, newDataSize);
        return newDataSize;
    }

    static int xferinfo(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
        Curl *curl = reinterpret_cast<Curl*>(p);

        return curl->getProgressCallback()((static_cast<double>(ulnow) / static_cast<double>(ultotal)) * 100);
    }

    EResultVal<std::string> Curl::get(std::string path, std::map<std::string, std::string> extraHeaders) {
        CURLcode result;
        std::string response;

        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Cache-Control: no-cache");

        if (extraHeaders.size() > 0)
            for (auto &[key, value] : extraHeaders)
                headers = curl_slist_append(headers, (key + ": " + value).c_str());

        curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(this->m_curl, CURLOPT_URL, std::string(this->m_baseURL + path).c_str());
        curl_easy_setopt(this->m_curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(this->m_curl, CURLOPT_WRITEFUNCTION, writeToString);
        curl_easy_setopt(this->m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(this->m_curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(this->m_curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(this->m_curl, CURLOPT_TIMEOUT_MS, 1000L);
        curl_easy_setopt(this->m_curl, CURLOPT_CONNECTTIMEOUT_MS, 1000L);

        result = curl_easy_perform(this->m_curl);

        if (result != CURLE_OK) {
            Logger::debug(curl_easy_strerror(result));
            return { ResultEdzCurlError, "" };
        }

        return { ResultSuccess, response };
    }
    
    EResultVal<std::string> Curl::post(std::string path, std::string body) {
        CURLcode result;
        std::string response;

        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Cache-Control: no-cache");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Accept: application/json");

        curl_easy_setopt(this->m_curl, CURLOPT_URL, std::string(this->m_baseURL + path).c_str());
        curl_easy_setopt(this->m_curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(this->m_curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(this->m_curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(this->m_curl, CURLOPT_WRITEFUNCTION, writeToString);
        curl_easy_setopt(this->m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(this->m_curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(this->m_curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(this->m_curl, CURLOPT_TIMEOUT_MS, 1000L);
        curl_easy_setopt(this->m_curl, CURLOPT_CONNECTTIMEOUT_MS, 1000L);
        curl_easy_setopt(this->m_curl, CURLOPT_HTTPHEADER, headers);

        result = curl_easy_perform(this->m_curl);

        if (result != CURLE_OK) {
            Logger::debug(curl_easy_strerror(result));
            return { ResultEdzCurlError, "" };
        }

        return { ResultSuccess, response };
    }


    EResultVal<std::vector<u8>> Curl::download(std::string path) {
        CURLcode result;
        std::vector<u8> data;

        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Cache-Control: no-cache");

        curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(this->m_curl, CURLOPT_URL, (this->m_baseURL + path).c_str());
        curl_easy_setopt(this->m_curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(this->m_curl, CURLOPT_WRITEFUNCTION, writeToBuffer);
        curl_easy_setopt(this->m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(this->m_curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(this->m_curl, CURLOPT_WRITEDATA, &data);
        curl_easy_setopt(this->m_curl, CURLOPT_XFERINFOFUNCTION, &xferinfo);
        curl_easy_setopt(this->m_curl, CURLOPT_XFERINFODATA, this);
        curl_easy_setopt(this->m_curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(this->m_curl, CURLOPT_TIMEOUT_MS, 1000L);
        curl_easy_setopt(this->m_curl, CURLOPT_CONNECTTIMEOUT_MS, 1000L);

        result = curl_easy_perform(this->m_curl);

        if (result != CURLE_OK) {
            Logger::debug(curl_easy_strerror(result));
            return { ResultEdzCurlError, std::vector<u8>() };
        }

        return { ResultSuccess, data };
    }

    EResult Curl::download(std::string path, std::string downloadPath) {
        CURLcode result;

        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Cache-Control: no-cache");

        FILE *file = fopen(downloadPath.c_str(), "wb");

        curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(this->m_curl, CURLOPT_URL, (this->m_baseURL + path).c_str());
        curl_easy_setopt(this->m_curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(this->m_curl, CURLOPT_WRITEFUNCTION, writeToFile);
        curl_easy_setopt(this->m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(this->m_curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(this->m_curl, CURLOPT_WRITEDATA, file);
        curl_easy_setopt(this->m_curl, CURLOPT_XFERINFOFUNCTION, &xferinfo);
        curl_easy_setopt(this->m_curl, CURLOPT_XFERINFODATA, this);
        curl_easy_setopt(this->m_curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(this->m_curl, CURLOPT_TIMEOUT_MS, 1000L);
        curl_easy_setopt(this->m_curl, CURLOPT_CONNECTTIMEOUT_MS, 1000L);

        result = curl_easy_perform(this->m_curl);

        fclose(file);

        if (result != CURLE_OK) {
            Logger::debug(curl_easy_strerror(result));
            return ResultEdzCurlError;
        }

        return ResultSuccess;
    }

    EResultVal<std::string> Curl::upload(std::string path, std::string fileName, std::vector<u8> &data) {
        CURLcode result;
        std::string response;

        curl_mime *mime = curl_mime_init(this->m_curl);
        curl_mimepart *part = curl_mime_addpart(mime);

        curl_mime_data(part, reinterpret_cast<const char*>(&data[0]), data.size());
        curl_mime_filename(part, fileName.c_str());
        curl_mime_name(part, "file");

        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Cache-Control: no-cache");

        curl_easy_setopt(this->m_curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(this->m_curl, CURLOPT_MIMEPOST, this);
        curl_easy_setopt(this->m_curl, CURLOPT_URL, (this->m_baseURL + path).c_str());
        curl_easy_setopt(this->m_curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(this->m_curl, CURLOPT_WRITEFUNCTION, writeToString);
        curl_easy_setopt(this->m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(this->m_curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(this->m_curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(this->m_curl, CURLOPT_XFERINFOFUNCTION, &xferinfo);
        curl_easy_setopt(this->m_curl, CURLOPT_XFERINFODATA, this);
        curl_easy_setopt(this->m_curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(this->m_curl, CURLOPT_TIMEOUT_MS, 1000L);
        curl_easy_setopt(this->m_curl, CURLOPT_CONNECTTIMEOUT_MS, 1000L);
        
        result = curl_easy_perform(this->m_curl);

        if (result != CURLE_OK) {
            Logger::debug(curl_easy_strerror(result));
            return { ResultEdzCurlError, "" };
        }

        return { ResultSuccess, response };
    }

    EResultVal<std::string> Curl::upload(std::string path, std::string uploadPath) {
        CURLcode result;
        std::string response;

        hlp::File file(uploadPath);
        std::vector<u8> data;
        data.reserve(file.size());
        file.read(&data[0], file.size());

        curl_mime *mime = curl_mime_init(this->m_curl);
        curl_mimepart *part = curl_mime_addpart(mime);

        curl_mime_data(part, reinterpret_cast<const char*>(&data[0]), file.size());
        curl_mime_filename(part, file.name().c_str());
        curl_mime_name(part, "file");

        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Cache-Control: no-cache");

        curl_easy_setopt(this->m_curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(this->m_curl, CURLOPT_MIMEPOST, this);
        curl_easy_setopt(this->m_curl, CURLOPT_URL, (this->m_baseURL + path).c_str());
        curl_easy_setopt(this->m_curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(this->m_curl, CURLOPT_WRITEFUNCTION, writeToString);
        curl_easy_setopt(this->m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(this->m_curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(this->m_curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(this->m_curl, CURLOPT_XFERINFOFUNCTION, &xferinfo);
        curl_easy_setopt(this->m_curl, CURLOPT_XFERINFODATA, this);
        curl_easy_setopt(this->m_curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(this->m_curl, CURLOPT_TIMEOUT_MS, 1000L);
        curl_easy_setopt(this->m_curl, CURLOPT_CONNECTTIMEOUT_MS, 1000L);
        
        result = curl_easy_perform(this->m_curl);

        if (result != CURLE_OK) {
            Logger::debug(curl_easy_strerror(result));
            return { ResultEdzCurlError, "" };
        }

        return { ResultSuccess, response };
    }


    std::function<bool(u8)>& Curl::getProgressCallback() {
        return this->m_progressCallback;
    }

    void Curl::setProgressCallback(std::function<bool(u8)> callback) {
        this->m_progressCallback = callback;
    }

}