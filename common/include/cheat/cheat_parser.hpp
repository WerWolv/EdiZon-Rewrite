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
#include "cheat/dmntcht.hpp"
#include <vector>

namespace edz::cheat {

    enum class ParseResult {
        NONE                         = 0x00,

        WARN_NAME_TOO_LONG           = 0x01,

        ERROR_TOO_MANY_OPCODES       = 0x81,
        ERROR_SYNTAX_ERROR           = 0x82,
        ERROR_CHEAT_EMPTY            = 0x84,
        ERROR_TOO_MANY_CHEATS        = 0x88,
        ERROR_MULTIPLE_MASTER_CHEATS = 0x90,
        ERROR_INVALID_OPCODE         = 0xA0,
        ERROR_NAME_NOT_TERMINATED    = 0xC0,
    };

    inline ParseResult operator|(ParseResult lhs, ParseResult rhs) {
        using T = std::underlying_type_t <ParseResult>;
        return static_cast<ParseResult>(static_cast<T>(lhs) | static_cast<T>(rhs));
    }

    inline ParseResult& operator|=(ParseResult& lhs, ParseResult rhs) {
        lhs = lhs | rhs;
        return lhs;
    }
    

    class CheatParser {
    public:
        CheatParser() = delete;
        CheatParser(CheatParser &) = delete;
        CheatParser& operator=(CheatParser &) = delete; 

        static std::pair<EResult, std::vector<dmntcht::CheatDefinition>> parseString(std::string content);
        static std::pair<EResult, std::vector<dmntcht::CheatDefinition>> parseFile(std::string filePath);

        static std::pair<EResult, std::string> disassembleCheat(dmntcht::CheatDefinition cheatDef);

        static ParseResult getParseResults() {
            return CheatParser::s_parseResults;
        }

    private:
        static inline enum ParseResult s_parseResults = ParseResult::NONE;
    };

}