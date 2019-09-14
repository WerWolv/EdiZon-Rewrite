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
#include <optional>

namespace edz::cheat {

    enum class ParseResult {
        NONE,

        WARN_NAME_TOO_LONG,

        ERROR_TOO_MANY_OPCODES,
        ERROR_SYNTAX_ERROR,
        ERROR_CHEAT_EMPTY  
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

        static std::pair<EResult, std::optional<dmntcht::CheatDefinition>> parseString(std::string content);
        static std::pair<EResult, std::optional<dmntcht::CheatDefinition>> parseFile(std::string filePath);

        static std::pair<EResult, std::string> disassembleCheat(dmntcht::CheatDefinition cheatDef);

        static ParseResult getParseResults() {
            return CheatParser::s_parseResults;
        }

    private:
        static enum ParseResult s_parseResults;
    };

}