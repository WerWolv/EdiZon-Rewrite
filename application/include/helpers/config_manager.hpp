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

#define SET_CONFIG(cfg, value)  do { edz::hlp::ConfigManager::getConfig().cfg = value; \
                                     edz::hlp::ConfigManager::store(); } while (false);

#define GET_CONFIG(cfg)              edz::hlp::ConfigManager::getConfig().cfg

namespace edz::hlp {

    typedef struct {
        struct {
            std::string buildTime;
            bool debugMode;
        } Common;

        struct {
            std::string localCommitSha;
            
            bool loggedIn;
            std::string switchcheatsdbEmail;
            std::string switchcheatsdbApiToken;
        } Update;

        struct {
            u8 titlesSortingStyle;
            std::vector<std::string> saveFileRepos;
        } Save;

        struct {
            std::vector<u32> favoriteColors;
        } VC;
    } config_t;

    class ConfigManager {
    public:       
        static config_t& getConfig();

        static void load();
        static void store();

    private:
        static inline config_t s_config;
    };

}