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

#include "helpers/config_manager.hpp"
#include <nlohmann/json.hpp>
#include "helpers/file.hpp"

namespace edz::hlp {

    using json = nlohmann::json;

    config_t& ConfigManager::getConfig() {
        return ConfigManager::s_config;
    }

    void ConfigManager::load() {
        json j;

        try {
            std::ifstream file(EDIZON_BASE_DIR"/config.json");
            file >> j;

            j.at("common").at("buildTime").get_to(ConfigManager::s_config.Common.buildTime);
            j.at("common").at("debugMode").get_to(ConfigManager::s_config.Common.debugMode);

            j.at("update").at("localCommitSha").get_to(ConfigManager::s_config.Update.localCommitSha);
            j.at("update").at("loggedIn").get_to(ConfigManager::s_config.Update.loggedIn);
            j.at("update").at("switchcheatsdbEmail").get_to(ConfigManager::s_config.Update.switchcheatsdbEmail);
            j.at("update").at("switchcheatsdbApiToken").get_to(ConfigManager::s_config.Update.switchcheatsdbApiToken);

            j.at("save").at("titlesDisplayStyle").get_to(ConfigManager::s_config.Save.titlesDisplayStyle);
            j.at("save").at("titlesSortingStyle").get_to(ConfigManager::s_config.Save.titlesSortingStyle);
            j.at("save").at("saveFileRepos").get_to(ConfigManager::s_config.Save.saveFileRepos);

            j.at("vc").at("favoriteColors").get_to(ConfigManager::s_config.VC.favoriteColors);
        } catch (std::exception &e) {
            Logger::info("Config file not found. Creating...");
            store();
        }

        // If the used edizon build has changed, the config might not be complete anymore so initialize the new values if there are any
        if (ConfigManager::s_config.Common.buildTime != __DATE__ " " __TIME__)
            store();
    }

    void ConfigManager::store() {
       json j = {   { "common", { { "debugMode", ConfigManager::s_config.Common.debugMode },
                                  { "buildTime", __DATE__ " " __TIME__ } } },
                    { "update", { { "localCommitSha", ConfigManager::s_config.Update.localCommitSha },
                                  { "loggedIn", ConfigManager::s_config.Update.loggedIn },
                                  { "switchcheatsdbEmail" , ConfigManager::s_config.Update.switchcheatsdbEmail },
                                  { "switchcheatsdbApiToken", ConfigManager::s_config.Update.switchcheatsdbApiToken } } },
                    { "save",   { { "saveFileRepos", ConfigManager::s_config.Save.saveFileRepos },
                                  { "titlesDisplayStyle", ConfigManager::s_config.Save.titlesDisplayStyle },
                                  { "titlesSortingStyle", ConfigManager::s_config.Save.titlesSortingStyle } } },
                    { "vc",     { { "favoriteColors", ConfigManager::s_config.VC.favoriteColors } } }
                };  

        std::ofstream file(EDIZON_BASE_DIR"/config.json");
        file << j;
    }

}