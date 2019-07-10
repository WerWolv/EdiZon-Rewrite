#pragma once

#include "file.hpp"

#include <string>
#include <vector>
#include <map>
#include <dirent.h>

namespace edz::helper {
    class Folder {
    public:
        Folder(std::string path);
        Folder(std::string path, std::string folderName);
        ~Folder();

        std::string path();
        std::string folderName();

        void copyTo(std::string newPath);
        void removeFolder();

        std::map<std::string, File> getFiles();
        std::map<std::string, Folder> getFolders();

    private:
        DIR *m_dir;
        std::string m_path;
        std::string m_folderName;

        void openDirectory();
        void closeDirectory();
        void rewindDirectory();
    };
}