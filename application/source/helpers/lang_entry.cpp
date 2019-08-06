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

#include "helpers/lang_entry.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

namespace edz {

    LangEntry::LangEntry(std::string unlocalizedString) : m_unlocalizedString(unlocalizedString) {
        static nlohmann::json json;
        static bool initialized = false;

        if (LangEntry::m_localizedStrings.find(unlocalizedString) != LangEntry::m_localizedStrings.end()) return;


        // On first call get current language code and load localization file
        if (!initialized) {
            u64 languageCode;
            setGetLanguageCode(&languageCode);

            std::ifstream langFile("romfs:/lang/" + std::string(reinterpret_cast<char*>(&languageCode)) + ".json");

            // If no localization file for the selected language was found, default to en-US (American English)
            if (!langFile.is_open())
                langFile.open("romfs:/lang/en-US.json");
            
            langFile >> json;

            initialized = true;
        }

        LangEntry::m_localizedStrings.insert({ unlocalizedString, json[unlocalizedString] });
    }

    std::string LangEntry::get() {
        return LangEntry::m_localizedStrings[this->m_unlocalizedString];
    }

}