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

namespace edz::hlp {

    class ConfigManager {
    public:
        static ConfigManager& get() {
            static ConfigManager instance;
            
            return instance;
        }

        ConfigManager(ConfigManager const&) = delete;
        void operator=(ConfigManager const&) = delete;    

        typedef struct {
            struct common {
                bool debugMode;
            };

            struct update {
                std::string localCommitSha;
                std::string switchcheatsdbEmail;
                std::string switchcheatsdbApiToken;
            };

            struct save {
                std::vector<std::string> saveFileRepos;
            };

            struct vc {
                std::vector<u32> favoriteColors;
            };
        } config_t;
        
        config_t& getConfig();

        void load();
        void store();

    private:
        ConfigManager();
        ~ConfigManager();

    };

}