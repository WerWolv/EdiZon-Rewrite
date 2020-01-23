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
#include <map>

namespace edz {

    class LangEntry {
    public:
        LangEntry(std::string localizationKey);

        std::string get() const;
        const char* c_str() const;

        operator std::string() const;
        std::string operator+(std::string other) const;
        std::string operator+(LangEntry other) const;

        static std::map<std::string, std::string> getSupportedLanguages();
        static void clearCache();

    private:
        std::string m_unlocalizedString;

        static inline std::map<std::string, std::string> s_localizedStrings;
        static inline std::map<std::string, std::string> s_englishLocalizedStrings;
    };

}

std::string operator+(std::string left, edz::LangEntry right);
std::string operator ""_lang (const char *localizationKey, size_t size);
