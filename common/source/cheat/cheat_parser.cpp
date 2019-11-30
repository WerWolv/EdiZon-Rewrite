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

    std::pair<EResult, std::vector<dmntcht::CheatDefinition>> CheatParser::parseString(std::string s) {
        CheatParser::s_parseResults = ParseResult::NONE;

        if (s.size() == 0) {
            CheatParser::s_parseResults |= ParseResult::ERROR_CHEAT_EMPTY;
            return { ResultEdzCheatParsingFailed, {} };
        }

        std::vector<dmntcht::CheatDefinition> cheatDefs;
        dmntcht::CheatDefinition *currCheatDef = nullptr;
        cheatDefs.push_back({ 0 }); // Master cheat

        size_t i = 0;
        size_t len = s.length() - 1;
        while (i < len) {
            if (std::isspace(static_cast<unsigned char>(s[i]))) {
                /* Just ignore whitespace. */
                i++;
            } else if (s[i] == '[') {
                /* Parse a readable cheat name. */
                cheatDefs.push_back({ 0 });
                currCheatDef = &cheatDefs.back();

                if (cheatDefs.size() >= 0x80) {
                    CheatParser::s_parseResults |= ParseResult::ERROR_TOO_MANY_CHEATS;
                    return { ResultEdzCheatParsingFailed, EMPTY_RESPONSE };
                }

                /* Extract name bounds. */
                size_t j = i + 1;
                while (s[j] != ']') {
                    j++;

                    if (j >= len) {
                        CheatParser::s_parseResults |= ParseResult::ERROR_NAME_NOT_TERMINATED;
                        return { ResultEdzCheatParsingFailed, EMPTY_RESPONSE };
                    }
                }

                /* s[i+1:j] is cheat name. */
                const size_t cheat_name_len = std::min(j - i - 1, sizeof(currCheatDef->readable_name));
                std::memcpy(currCheatDef->readable_name, &s[i+1], cheat_name_len);
                currCheatDef->readable_name[cheat_name_len] = 0;

                if (j - i - 1 > 0x3F)
                    CheatParser::s_parseResults |= ParseResult::WARN_NAME_TOO_LONG;

                /* Skip onwards. */
                i = j + 1;
            } else if (s[i] == '{') {
                /* We're parsing a master cheat. */
                currCheatDef = &cheatDefs[0];

                /* There can only be one master cheat. */
                if (currCheatDef->num_opcodes > 0) {
                    CheatParser::s_parseResults |= ParseResult::ERROR_MULTIPLE_MASTER_CHEATS;
                    return { ResultEdzCheatParsingFailed, EMPTY_RESPONSE };
                }

                /* Extract name bounds */
                size_t j = i + 1;
                while (s[j] != '}') {
                    j++;
                    if (j >= len) {
                        CheatParser::s_parseResults |= ParseResult::ERROR_NAME_NOT_TERMINATED;
                        return { ResultEdzCheatParsingFailed, EMPTY_RESPONSE };
                    }
                }

                /* s[i+1:j] is cheat name. */
                const size_t cheat_name_len = std::min(j - i - 1, sizeof(currCheatDef->readable_name));
                memcpy(currCheatDef->readable_name, &s[i+1], cheat_name_len);
                currCheatDef->readable_name[cheat_name_len] = 0;

                /* Skip onwards. */
                i = j + 1;
            } else if (std::isxdigit(static_cast<unsigned char>(s[i]))) {
                /* Bounds check the opcode count. */
                if (currCheatDef->num_opcodes >= std::size(currCheatDef->opcodes)) {
                    CheatParser::s_parseResults |= ParseResult::ERROR_TOO_MANY_OPCODES;
                    return { ResultEdzCheatParsingFailed, EMPTY_RESPONSE };
                }

                /* We're parsing an instruction, so validate it's 8 hex digits. */
                for (size_t j = 1; j < 8; j++) {
                    /* Validate 8 hex chars. */

                    if (i + j >= len || !std::isxdigit(static_cast<unsigned char>(s[i+j]))) {
                        CheatParser::s_parseResults |= ParseResult::ERROR_INVALID_OPCODE;
                        return { ResultEdzCheatParsingFailed, EMPTY_RESPONSE };
                    }
                }

                /* Parse the new opcode. */
                char hex_str[9] = {0};
                std::memcpy(hex_str, &s[i], 8);
                currCheatDef->opcodes[currCheatDef->num_opcodes++] = std::strtoul(hex_str, NULL, 16);

                /* Skip onwards. */
                i += 8;
            } else {
                /* Unexpected character encountered. */
                printf("%x\n", s[i]);
                CheatParser::s_parseResults |= ParseResult::ERROR_SYNTAX_ERROR;
                return { ResultEdzCheatParsingFailed, EMPTY_RESPONSE };
            }
        }

        return { ResultSuccess, cheatDefs };
    }

    std::pair<EResult, std::vector<dmntcht::CheatDefinition>> CheatParser::parseFile(std::string filePath) {
        hlp::File file(filePath);

        return CheatParser::parseString(file.read());
    }

}