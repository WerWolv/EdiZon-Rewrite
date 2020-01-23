/**
 * Copyright (C) 2020 WerWolv
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
#include <mutex>

#define SET_CONFIG(cfg, value)  do { edz::hlp::ConfigManager::getConfig().cfg = value; \
                                     edz::hlp::ConfigManager::store(); } while (false);

#define GET_CONFIG(cfg)              edz::hlp::ConfigManager::getConfig().cfg

namespace edz::hlp {

    typedef struct {
        struct {
            std::string buildTime;
        } Common;

        struct {
            std::string localCommitSha;
            std::vector<time_t> notificationDates;

            bool loggedIn;
            std::string switchcheatsdbEmail;
            std::string switchcheatsdbApiToken;
        } Online;

        struct {
            u16 searchCount;
        } CheatEngine;

        struct {
            std::string langCode;
            bool pctlChecksEnabled;

            u8 titlesDisplayStyle;
            u8 titlesSortingStyle;

            std::vector<std::string> saveFileRepos;

            bool fennecMode;
        } Settings;
    } config_t;

    class ConfigManager {
    public:       
        static config_t& getConfig();

        static void load();
        static void store();

    private:
        static inline config_t s_config;

        static inline std::mutex s_mutex;
    };

}