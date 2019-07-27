#include "helpers/curl.hpp"

#include <cstring>

namespace edz::helper {

    Curl::Curl(std::string baseURL) : m_baseURL(baseURL) {
        this->m_curl = curl_easy_init();
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

    EResult Curl::get(std::string path, std::string &response) {
        CURLcode result;

        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Cache-Control: no-cache");

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
            debug(curl_easy_strerror(result));
            return EResult(MODULE_EDIZON, 1);
        }

        return EResult(0, 0);
    }

    EResult Curl::download(std::string path, std::vector<u8> &data, std::string &response) {
        CURLcode result;

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
            debug(curl_easy_strerror(result));
            return EResult(MODULE_EDIZON, 1);
        }

        return EResult(0, 0);
    }

    EResult Curl::download(std::string path, std::string downloadPath, std::string &response) {
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
            debug(curl_easy_strerror(result));
            return EResult(MODULE_EDIZON, 1);
        }

        return EResult(0, 0);
    }

    EResult Curl::upload(std::string path, std::string fileName, std::vector<u8> &data, std::string &response) {
        CURLcode result;
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
            debug(curl_easy_strerror(result));
            return EResult(MODULE_EDIZON, 1);
        }

        return EResult(0, 0);
    }


    std::function<bool(u8)>& Curl::getProgressCallback() {
        return this->m_progressCallback;
    }

    void Curl::setProgressCallback(std::function<bool(u8)> callback) {
        this->m_progressCallback = callback;
    }

}