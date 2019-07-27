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