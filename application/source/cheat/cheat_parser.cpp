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

#include "cheat/cheat_parser.hpp"
#include "helpers/file.hpp"
#include <cctype>

namespace edz::cheat {

    std::pair<EResult, std::optional<dmntcht::CheatDefinition>> CheatParser::parseString(std::string content) {
        if (content.size() == 0) return { ResultEdzCheatParsingFailed, {} };

        dmntcht::CheatDefinition cheatDef = { 0 };

        char *pos = &content[0];
        while (pos != (&content[0] + content.size())) {

            // Parse cheat name
            if (*pos == '[') {
                for (u32 i = 0; i < content.size() && *pos != ']'; i++)
                    if (i < 0x40)
                        cheatDef.readable_name[i] = *(pos++);
            }
            else if (isspace) {
                pos++;
            }
        }
    }

    std::pair<EResult, std::optional<dmntcht::CheatDefinition>> CheatParser::parseFile(std::string filePath) {
        hlp::File file(filePath);

        return CheatParser::parseString(file.read());
    }

}