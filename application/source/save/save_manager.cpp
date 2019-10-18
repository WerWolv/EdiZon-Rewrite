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

#include "save/save_manager.hpp"
#include "save/save_data.hpp"
#include "helpers/utils.hpp"
#include "api/anonfile_api.hpp"
#include "api/switchcheatsdb_api.hpp"

#include <errno.h>
#include <string.h>
#include <regex>
#include <vector>
#include <filesystem>

#include "zipper.h"
#include "unzipper.h"

namespace edz::save {

    EResult SaveManager::backup(Title *title, Account *account, std::string backupName, std::string basePath) {
        hlp::File backupFile;

        if (basePath == "")
            backupFile = hlp::File(hlp::formatString("%s/%s/%s.edz", EDIZON_BACKUP_DIR, SaveManager::getBackupFolderName(title).c_str(), backupName.c_str()));
        else
            backupFile = hlp::File(hlp::formatString("%s/%s.edz", basePath.c_str(), backupName.c_str()));

        if (!title->hasSaveFile(account))
            return ResultEdzSaveNoSaveFS;

        backupFile.createDirectories();

        save::SaveFileSystem saveFS(title, account);
        hlp::Folder saveFSFolder = saveFS.getSaveFolder();

        // Strip away the leading save_#:/ so Zipper creates the zip file properly
        size_t filePathOffset = 0;
        if ((filePathOffset = saveFSFolder.path().find(':')) == std::string::npos)
            return ResultEdzSaveNoSaveFS;
        else
            filePathOffset += 2; // Skip to after the first /

        std::vector<u8> zipData;
        zipper::Zipper zip(zipData);

        // Zip all files and folders in the save FS
        for (auto &it : std::filesystem::recursive_directory_iterator(saveFS.getSaveFolder().path())) {
            if (!it.is_directory()) {
                std::ifstream saveFsStream = std::ifstream(it.path());
                zip.add(saveFsStream, &(it.path().c_str()[filePathOffset]), zipper::Zipper::Better);
            }
        }

        zip.close();

        backup_header_t header = { 0x4E5A4445, title->getID(), account->getID(), time(nullptr) };
        std::vector<u8> backupData;

        // Add a header to the backup file so EdiZon has some more information when downloading them from the internet
        std::copy(reinterpret_cast<u8*>(&header), reinterpret_cast<u8*>(&header) + sizeof(backup_header_t), std::back_inserter(backupData));

        // Add the zip data after the header. Zipper thows an exception if the vector passed in is not empty...
        std::copy(zipData.begin(), zipData.end(), std::back_inserter(backupData));

        backupFile.write(&backupData[0], backupData.size());

        return ResultSuccess;
    }

    EResult SaveManager::restore(Title *title, Account *account, std::string backupName, std::string basePath) {
        hlp::File backupFile;

        if (basePath == "")
            backupFile = hlp::File(hlp::formatString("%s/%s/%s" BACKUP_FILE_EXTENSION, EDIZON_BACKUP_DIR, SaveManager::getBackupFolderName(title).c_str(), backupName.c_str()));
        else
            backupFile = hlp::File(hlp::formatString("%s/%s" BACKUP_FILE_EXTENSION, basePath.c_str(), backupName.c_str()));

        if (!backupFile.exists())
            return ResultEdzSaveNoSuchBackup;

        if (!title->hasSaveFile(account))
            return ResultEdzSaveNoSaveFS;


        save::SaveFileSystem saveFS(title, account);

        // Zipper (and apparently most other zipping tools too) don't care at all about the extra header. So we can just unzip it
        hlp::Folder tmpFolder = hlp::createTmpFolder();
        zipper::Unzipper unzip(backupFile.path());
        unzip.extract(tmpFolder.path());

        saveFS.getSaveFolder().remove();
        saveFS.commit();
        
        tmpFolder.copyTo(saveFS.getSaveFolder().path());

        // Copy all files and folders manually to avoid copying the parent folder to the save FS 
        for (auto [name, file] : tmpFolder.getFiles()) {
            file.copyTo(saveFS.getSaveFolder().path() + name);
            saveFS.commit();
        }

        for (auto [name, folder] : tmpFolder.getFolders()) {
            folder.copyTo(saveFS.getSaveFolder().path() + name);
            saveFS.commit();
        }

        tmpFolder.remove();

        return ResultSuccess;
    }

    EResult SaveManager::swapSaveData(Title *title, Account *account, std::string backupName) {
        hlp::File backupFile = hlp::File(hlp::formatString("%s/%s/%s" BACKUP_FILE_EXTENSION, EDIZON_BACKUP_DIR, SaveManager::getBackupFolderName(title).c_str(), backupName.c_str()));
        hlp::File tmpFile = hlp::File(hlp::createTmpFolder().path() + backupName);

        if (!backupFile.exists())
            return ResultEdzSaveNoSuchBackup;

        if (!title->hasSaveFile(account))
            return ResultEdzSaveNoSaveFS;

        save::SaveFileSystem saveFS(title, account);
        hlp::Folder saveFolder = saveFS.getSaveFolder();

        ER_TRY(SaveManager::backup(title, account, backupName, tmpFile.parent()));
        ER_TRY(SaveManager::restore(title, account, backupName));

        backupFile.remove();
        tmpFile.copyTo(backupFile.path());
        tmpFile.remove();

        return ResultSuccess;
    }

    EResult SaveManager::swapSaveData(Title *title, Account *account1, Account *account2) {
        hlp::Folder tmpFolder = hlp::createTmpFolder();

        if (!title->hasSaveFile(account1))
            return ResultEdzSaveNoSaveFS;

        save::SaveFileSystem saveFS1(title, account1);
        save::SaveFileSystem saveFS2(title, account2);

        hlp::Folder saveFolder1 = saveFS1.getSaveFolder();
        hlp::Folder saveFolder2 = saveFS2.getSaveFolder();
        
        // Copy save files from first account's save FS to a temporary folder
        for (auto [name, file] : saveFolder1.getFiles())
            file.copyTo(tmpFolder.path() + name);
        for (auto [name, folder] : saveFolder1.getFolders())
            folder.copyTo(tmpFolder.path() + name);

        saveFolder1.remove();

        // Copy save files from second account's save FS to first account's save FS
        for (auto [name, file] : saveFolder2.getFiles()) {
            file.copyTo(saveFolder1.path() + name);
            saveFS1.commit();
        }
        for (auto [name, folder] : saveFolder2.getFolders()) {
            folder.copyTo(saveFolder1.path() + name);
            saveFS1.commit();
        }

        saveFolder2.remove();

        // Copy save files from temporary folder to second account's save FS
        for (auto [name, file] : tmpFolder.getFiles()) {
            file.copyTo(saveFolder2.path() + name);
            saveFS2.commit();
        }
        for (auto [name, folder] : tmpFolder.getFolders()) {
            folder.copyTo(saveFolder2.path() + name);
            saveFS2.commit();
        }

        tmpFolder.remove();

        return ResultSuccess;
    }

    EResult SaveManager::remove(Title *title, Account *account) {
        save::SaveFileSystem saveFS(title, account);

        EResult res = saveFS.getSaveFolder().remove();
        saveFS.commit();

        return res;
    }

    EResult SaveManager::duplicate(Title *title, Account *from, Account *to) {
        if (!title->hasSaveFile(from))
            return ResultEdzSaveNoSaveFS;

        if (!title->hasSaveFile(to))
            return ResultEdzSaveNoSaveFS;

        save::SaveFileSystem saveFSFrom(title, from);
        save::SaveFileSystem saveFSTo(title, to);

        hlp::Folder saveFolderFrom = saveFSFrom.getSaveFolder();
        hlp::Folder saveFolderTo = saveFSTo.getSaveFolder();

        saveFolderTo.remove();
        saveFSTo.commit();

        for (auto [name, file] : saveFolderFrom.getFiles()) {
            file.copyTo(saveFolderTo.path() + name);
            saveFSTo.commit();
        }
        for (auto [name, folder] : saveFolderFrom.getFolders()) {
            folder.copyTo(saveFolderTo.path() + name);
            saveFSTo.commit();
        }

        return ResultSuccess;
    }


    EResult SaveManager::upload(Title *title, Account *account, std::string backupName) {
        api::AnonfileAPI anonfileApi;
        hlp::File tmpFile = hlp::File(hlp::createTmpFolder().path() + backupName);

        SaveManager::backup(title, account, backupName, tmpFile.parent());

        auto [result, link] = anonfileApi.upload(tmpFile.path());
        if (result.succeeded()) {
            api::SwitchCheatsDBAPI switchCheatDBApi;
            switchCheatDBApi.addSaveFile(backupName, link, title);
        }

        return result;
    }

    EResult SaveManager::upload(Title *title, std::string backupName) {
        api::AnonfileAPI anonfileApi;
        hlp::File backupFile = hlp::File(hlp::formatString("%s/%s/%s" BACKUP_FILE_EXTENSION, EDIZON_BACKUP_DIR, SaveManager::getBackupFolderName(title).c_str(), backupName.c_str()));

        auto [result, link] = anonfileApi.upload(backupFile.path());
        if (result.succeeded()) {
            api::SwitchCheatsDBAPI switchCheatDBApi;
            switchCheatDBApi.addSaveFile(backupName, link, title);
        }

        return result;
    }


    EResult SaveManager::download(Title *title, Account *account, std::string url) {
        hlp::Curl curl(url);
        hlp::File tmpFile(hlp::createTmpFolder().path() + "backup" BACKUP_FILE_EXTENSION);

        if (curl.download("", tmpFile.path()).failed())
            return ResultEdzAPIError;

        ER_TRY(SaveManager::restore(title, account, "backup", tmpFile.parent()).failed());

        tmpFile.remove();

        return ResultSuccess;
    }
    
    EResult SaveManager::download(Title *title, std::string localName, std::string url) {
        hlp::Curl curl(url);
        hlp::File backupFile = hlp::File(hlp::formatString("%s/%s/%s" BACKUP_FILE_EXTENSION, EDIZON_BACKUP_DIR, SaveManager::getBackupFolderName(title).c_str(), localName.c_str()));

        return curl.download("", backupFile.path());
    }
    

    std::pair<EResult, std::vector<std::string>> SaveManager::getLocalBackupList(Title *title) {
        std::regex regex = std::regex(".+ " + title->getIDString());

        hlp::Folder backupFolder(hlp::formatString("%s/%s", EDIZON_BACKUP_DIR, SaveManager::getBackupFolderName(title).c_str()));

        std::vector<std::string> backupFiles;

        for (auto &[fileName, file] : backupFolder.getFiles()) {
            backupFiles.push_back(fileName.substr(0, fileName.size() - 4));
        }

        return { ResultSuccess, backupFiles }; 
    }
    
    std::pair<EResult, std::map<std::string, std::string>> SaveManager::getOnlineBackupList(Title *title) {
        api::SwitchCheatsDBAPI switchCheatsDBApi;
        std::map<std::string, std::string> saveFileNames;

        auto [result, saveFiles] = switchCheatsDBApi.getSaveFiles();

        if (result.failed())
            return { ResultEdzAPIError, EMPTY_RESPONSE };

        for (auto &saveFile : saveFiles)
            saveFileNames.insert({ saveFile.name, saveFile.url });

        return { ResultSuccess, saveFileNames };
    }
    
    std::pair<EResult, bool> SaveManager::areBackupsUpToDate(Title *title, Account *account) {
        return { ResultEdzNotYetImplemented, false };
    }


    std::string SaveManager::getBackupFolderName(Title *title) {
        // Find a folder that matches the following name: <Any alpha numberical string> <TitleID>
        std::regex regex = std::regex(".+ " + title->getIDString());
        std::string backupFolderName;
        for (auto &[folderName, folder] : hlp::Folder(EDIZON_BACKUP_DIR).getFolders())
            if (std::regex_match(folderName, regex)) {
                backupFolderName = folderName;
                break;
            }
        
        // Set the default name to <Title Name> <TitleID>
        if (backupFolderName == "")
            backupFolderName = hlp::removeIllegalPathCharacters(title->getName()) + " " + title->getIDString();

        return backupFolderName;
    }
    

}