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
        std::lock_guard<std::mutex> lock(ConfigManager::s_mutex);

        json j;

        try {
            std::ifstream file(EDIZON_BASE_DIR "/config.json");
            file >> j;

            j.at("common").at("buildTime").get_to(ConfigManager::s_config.Common.buildTime);

            j.at("online").at("localCommitSha").get_to(ConfigManager::s_config.Online.localCommitSha);
            j.at("online").at("notificationDates").get_to(ConfigManager::s_config.Online.notificationDates);
            j.at("online").at("loggedIn").get_to(ConfigManager::s_config.Online.loggedIn);
            j.at("online").at("switchcheatsdbEmail").get_to(ConfigManager::s_config.Online.switchcheatsdbEmail);
            j.at("online").at("switchcheatsdbApiToken").get_to(ConfigManager::s_config.Online.switchcheatsdbApiToken);

            j.at("settings").at("langCode").get_to(ConfigManager::s_config.Settings.langCode);
            j.at("settings").at("pctlChecksEnabled").get_to(ConfigManager::s_config.Settings.pctlChecksEnabled);
            j.at("settings").at("titlesDisplayStyle").get_to(ConfigManager::s_config.Settings.titlesDisplayStyle);
            j.at("settings").at("titlesSortingStyle").get_to(ConfigManager::s_config.Settings.titlesSortingStyle);
            j.at("settings").at("saveFileRepos").get_to(ConfigManager::s_config.Settings.saveFileRepos);
            j.at("settings").at("sysmoduleAutoStart").get_to(ConfigManager::s_config.Settings.sysmoduleAutoStart);
            j.at("settings").at("fennecMode").get_to(ConfigManager::s_config.Settings.fennecMode);

            j.at("cheatEngine").at("searchCount").get_to(ConfigManager::s_config.CheatEngine.searchCount);
        } catch (std::exception &e) {
            Logger::info("Config file not found. Creating...");
            store();
        }

        // If the used edizon build has changed, the config might not be complete anymore so initialize the new values if there are any
        if (ConfigManager::s_config.Common.buildTime != __DATE__ " " __TIME__)
            store();
    }

    void ConfigManager::store() {
        std::lock_guard<std::mutex> lock(ConfigManager::s_mutex);

        json j = {  { "common",      { { "buildTime",               __DATE__ " " __TIME__                                   } } },
                    { "online",      { { "localCommitSha",          ConfigManager::s_config.Online.localCommitSha           },
                                       { "notificationDates",       ConfigManager::s_config.Online.notificationDates        },
                                       { "loggedIn",                ConfigManager::s_config.Online.loggedIn                 },
                                       { "switchcheatsdbEmail" ,    ConfigManager::s_config.Online.switchcheatsdbEmail      },
                                       { "switchcheatsdbApiToken",  ConfigManager::s_config.Online.switchcheatsdbApiToken   } } },
                    { "settings",    { { "langCode",                ConfigManager::s_config.Settings.langCode               },
                                       { "saveFileRepos",           ConfigManager::s_config.Settings.saveFileRepos          },
                                       { "titlesDisplayStyle",      ConfigManager::s_config.Settings.titlesDisplayStyle     },
                                       { "titlesSortingStyle",      ConfigManager::s_config.Settings.titlesSortingStyle     },
                                       { "pctlChecksEnabled",       ConfigManager::s_config.Settings.pctlChecksEnabled      },
                                       { "sysmoduleAutoStart",      ConfigManager::s_config.Settings.sysmoduleAutoStart     },
                                       { "fennecMode",              ConfigManager::s_config.Settings.fennecMode             } } },
                    { "cheatEngine", { { "searchCount",             ConfigManager::s_config.CheatEngine.searchCount         } } }
                };  

        std::ofstream file(EDIZON_BASE_DIR "/config.json");
        file << j;
    }

}