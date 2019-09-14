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
#include <cstring>

namespace edz::cheat {

    std::pair<EResult, std::optional<dmntcht::CheatDefinition>> CheatParser::parseString(std::string content) {
        CheatParser::s_parseResults = ParseResult::NONE;

        if (content.size() == 0) {
            CheatParser::s_parseResults |= ParseResult::ERROR_CHEAT_EMPTY;
            return { ResultEdzCheatParsingFailed, {} };
        }

        dmntcht::CheatDefinition cheatDef = { 0 };

        char* pos = &content[0];
        while (pos < (&content[0] + content.size())) {

            // Parse cheat name
            if (*pos == '[') {
                if (cheatDef.num_opcodes != 0) break;

                pos++;

                memset(&cheatDef, 0x00, sizeof(dmntcht::CheatDefinition));

                for (uint32_t i = 0; i < content.size() && *pos != ']'; i++) {
                    if (i < 0x3F)
                        cheatDef.readable_name[i] = *pos;
                    else
                        CheatParser::s_parseResults |= ParseResult::WARN_NAME_TOO_LONG;
                    pos++;

                    if (*pos == '\n') {
                        CheatParser::s_parseResults |= ParseResult::ERROR_SYNTAX_ERROR;
                        return { ResultEdzCheatParsingFailed, {} };
                    }
                }

                if (*pos == ']')
                    pos++;
            }
            else if (isspace(*pos)) {
                pos++;
            }
            else if (isxdigit(*pos)) {
                if (cheatDef.num_opcodes >= 0x100) {
                    CheatParser::s_parseResults |= ParseResult::ERROR_TOO_MANY_OPCODES;
                    return { ResultEdzCheatParsingFailed, {} };
                }

                for (uint8_t i = 0; i < 8; i++)
                    if (!isxdigit(*(pos + i))) {
                        CheatParser::s_parseResults |= ParseResult::ERROR_SYNTAX_ERROR;
                        return { ResultEdzCheatParsingFailed, {} };
                    }

                cheatDef.opcodes[cheatDef.num_opcodes++] = strtoul(pos, &pos, 16);
            }
        }

        return { ResultSuccess, cheatDef };
    }

    std::pair<EResult, std::optional<dmntcht::CheatDefinition>> CheatParser::parseFile(std::string filePath) {
        hlp::File file(filePath);

        return CheatParser::parseString(file.read());
    }

}