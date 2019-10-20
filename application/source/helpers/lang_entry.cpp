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

    LangEntry::LangEntry(std::string localizationKey) : m_unlocalizedString(localizationKey) {
        if (this->m_localizedStrings.size() > 0) return;

        u64 languageCode;
        setGetLanguageCode(&languageCode);

        nlohmann::json json;
        std::ifstream langFile("romfs:/lang/" + std::string(reinterpret_cast<char*>(&languageCode)) + ".json");

        // If no localization file for the selected language was found, default to en-US (American English)
        if (!langFile.is_open())
            langFile.open("romfs:/lang/en-US.json");
        
        langFile >> json;

        for (auto &[unlocalizedString, localizedString] : json.items())
            LangEntry::m_localizedStrings.insert({ unlocalizedString, localizedString });
    }

    std::string LangEntry::get() const {
        if (LangEntry::m_localizedStrings.find(this->m_unlocalizedString) != LangEntry::m_localizedStrings.end())
            return LangEntry::m_localizedStrings[this->m_unlocalizedString];
        else return this->m_unlocalizedString;
    }

    const char* LangEntry::c_str() const {
        return get().c_str();
    }

    LangEntry::operator std::string() const {
        return get();
    }

    std::string LangEntry::operator+(std::string other) const {
        return get() + other;
    }

    std::string LangEntry::operator+(LangEntry other) const {
        return get() + other.get();
    }

}

std::string operator+(std::string left, edz::LangEntry right) {
    return left + right.get();
}

std::string operator ""_lang (const char *localizationKey, size_t size) {
    return edz::LangEntry(std::string(localizationKey, size)).get();
}