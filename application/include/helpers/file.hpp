#pragma once
#include "edizon.hpp"

#include <string>
#include <fstream>
#include <filesystem>

namespace edz::helper {

    class File {
    public:
        File(std::string filePath);
        File(const File &file);
        ~File();

        std::string fileName();
        std::string filePath();
        std::string basePath();

        size_t fileSize();

        void removeFile();
        void copyTo(std::string path);
        void createDirectories();

        bool valid();

        s32 read(u8 *buffer, size_t bufferSize);
        s32 write(u8 *buffer, size_t bufferSize);

    private:
        FILE *m_file;
        std::string m_filePath;

        void openFile();
        void closeFile();
    };

}