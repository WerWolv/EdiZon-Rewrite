/**
 * Copyright (C) 2019 - 2020 WerWolv
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

#include "helpers/file.hpp"
#include "helpers/config_manager.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

namespace edz {

    LangEntry::LangEntry(std::string localizationKey) : m_unlocalizedString(localizationKey) {
        if (this->s_localizedStrings.size() > 0) return;

        u64 languageCode;
        setGetLanguageCode(&languageCode);
        std::string overrideLanguage = GET_CONFIG(Settings.langCode);

        // If override language hasn't been set yet, assume operating system default
        if (overrideLanguage == "")
            SET_CONFIG(Settings.langCode, "auto");

        nlohmann::json json;
        std::ifstream langFile("romfs:/lang/" + (overrideLanguage == "auto" ? std::string(reinterpret_cast<char*>(&languageCode)) : overrideLanguage) + ".json");

        // If no localization file for the selected language was found, default to en-US (American English)
        if (!langFile.is_open())
            langFile.open("romfs:/lang/en-US.json");
        
        langFile >> json;

        for (auto &[unlocalizedString, localizedString] : json.items())
            LangEntry::s_localizedStrings.insert({ unlocalizedString, localizedString });

        // Insert missing localized strings from the english file since it's the most complete
        langFile.open("romfs:/lang/en-US.json");
        for (auto &[unlocalizedString, localizedString] : json.items())
            LangEntry::s_localizedStrings.insert({ unlocalizedString, localizedString });

    }

    std::string LangEntry::get() const {
        if (LangEntry::s_localizedStrings.find(this->m_unlocalizedString) != LangEntry::s_localizedStrings.end())
            return LangEntry::s_localizedStrings[this->m_unlocalizedString];
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

    std::map<std::string, std::string> LangEntry::getSupportedLanguages() {
        std::map<std::string, std::string> languages;
        std::ifstream languagesFile("romfs:/lang/languages.json");
        nlohmann::json languagesJson;

        languagesFile >> languagesJson;

        for (auto &[langCode, langName] : languagesJson.items()) {
            if (hlp::File("romfs:/lang/" + langCode + ".json").exists())
                languages.insert({ langCode, langName });
        }

        return languages;
    }

    void LangEntry::clearCache() {
        LangEntry::s_localizedStrings.clear();
    }

}

std::string operator+(std::string left, edz::LangEntry right) {
    return left + right.get();
}

std::string operator ""_lang (const char *localizationKey, size_t size) {
    return edz::LangEntry(std::string(localizationKey, size)).get();
}