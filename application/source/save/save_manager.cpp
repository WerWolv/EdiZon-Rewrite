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
#include "helpers/utils.hpp"
#include "save/save_data.hpp"

namespace edz::save {

    EResult SaveManager::backup(Title *title, Account *account, std::string backupName) {
        hlp::Folder backupFolder(hlp::formatString("%s/%s %s/%s", EDIZON_BACKUP_DIR, title->getIDString().c_str(), title->getName().c_str(), backupName.c_str()));
        
        if (!title->hasSaveFile(account))
            return ResultEdzSaveNoSaveFS;

        backupFolder.createDirectories();

        save::SaveFileSystem saveFS(title, account);
        saveFS.getSaveFolder().copyTo(backupFolder.path());

        return ResultSuccess;
    }

    EResult SaveManager::restore(Title *title, Account *account, std::string backupName) {
        hlp::Folder backupFolder(hlp::formatString("%s/%s %s/%s", EDIZON_BACKUP_DIR, title->getIDString().c_str(), title->getName().c_str(), backupName.c_str()));
        
        if (!backupFolder.exists())
            return ResultEdzSaveNoSuchBackup;

        if (!title->hasSaveFile(account))
            return ResultEdzSaveNoSaveFS;

        save::SaveFileSystem saveFS(title, account);
        saveFS.getSaveFolder().copyFrom(backupFolder.path());

        return ResultSuccess;
    }


    EResult SaveManager::swapSaveData(Title *title, Account *account, std::string backupName) {
        hlp::Folder backupFolder(hlp::formatString("%s/%s %s/%s", EDIZON_BACKUP_DIR, title->getIDString().c_str(), title->getName().c_str(), backupName.c_str()));
        hlp::Folder tmpFolder = hlp::createTmpFolder();

        if (!backupFolder.exists())
            return ResultEdzSaveNoSuchBackup;

        if (!title->hasSaveFile(account))
            return ResultEdzSaveNoSaveFS;

        save::SaveFileSystem saveFS(title, account);
        hlp::Folder saveFolder = saveFS.getSaveFolder();

        saveFolder.copyTo(tmpFolder.path());
        backupFolder.copyTo(saveFolder.path());
        tmpFolder.copyTo(backupFolder.path());

        return ResultSuccess;
    }

    EResult SaveManager::swapSaveData(Title *title, Account *account1, Account *account2) {
        hlp::Folder backupFolder(hlp::formatString("%s/%s %s", EDIZON_BACKUP_DIR, title->getIDString().c_str(), title->getName().c_str()));
        hlp::Folder tmpFolder = hlp::createTmpFolder();

        if (!backupFolder.exists())
            return ResultEdzSaveNoSuchBackup;

        if (!title->hasSaveFile(account1))
            return ResultEdzSaveNoSaveFS;

        save::SaveFileSystem saveFS1(title, account1);
        save::SaveFileSystem saveFS2(title, account2);

        hlp::Folder saveFolder1 = saveFS1.getSaveFolder();
        hlp::Folder saveFolder2 = saveFS2.getSaveFolder();
        
        saveFolder1.copyTo(tmpFolder.path());
        saveFolder2.copyTo(saveFolder1.path());
        tmpFolder.copyTo(saveFolder2.path());

        return ResultSuccess;
    }

    EResult SaveManager::duplicate(Title *title, Account *from, Account *to) {
        hlp::Folder backupFolder(hlp::formatString("%s/%s %s", EDIZON_BACKUP_DIR, title->getIDString().c_str(), title->getName().c_str()));
        hlp::Folder tmpFolder = hlp::createTmpFolder();

        if (!backupFolder.exists())
            return ResultEdzSaveNoSuchBackup;

        if (!title->hasSaveFile(from))
            return ResultEdzSaveNoSaveFS;

        if (!title->hasSaveFile(to))
            return ResultEdzSaveNoSaveFS;

        save::SaveFileSystem saveFSFrom(title, from);
        save::SaveFileSystem saveFSTo(title, to);

        hlp::Folder saveFolderFrom = saveFSFrom.getSaveFolder();
        hlp::Folder saveFolderTo = saveFSTo.getSaveFolder();

        saveFolderFrom.copyTo(saveFolderTo.path());

        return ResultSuccess;
    }


    EResult SaveManager::upload(Title *title, Account *account) {
        return ResultEdzNotYetImplemented;
    }

    EResult SaveManager::upload(Title *title, std::string backupName) {
        return ResultEdzNotYetImplemented;
    }


    EResult SaveManager::download(Title *title, Account *account) {
        return ResultEdzNotYetImplemented;
    }
    
    EResult SaveManager::download(Title *title, std::string backupName) {
        return ResultEdzNotYetImplemented;
    }
    

    std::pair<EResult, std::vector<std::string>> SaveManager::getLocalBackupList(Title *title) {
        hlp::Folder backupFolder(hlp::formatString("%s/%s %s", EDIZON_BACKUP_DIR, title->getIDString().c_str(), title->getName().c_str()));

        std::vector<std::string> folders;

        for (auto &[folderName, folder] : backupFolder.getFolders())
            folders.push_back(folderName);

        return { ResultSuccess, folders }; 
    }
    
    std::pair<EResult, std::vector<std::string>> SaveManager::getOnlineBackupList(Title *title) {
        return { ResultEdzNotYetImplemented, {} };
    }
    
    std::pair<EResult, bool> SaveManager::areBackupsUpToDate(Title *title, Account *account) {
        return { ResultEdzNotYetImplemented, false };
    }
    

}