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

#include "helpers/result.hpp"
#include "helpers/utils.hpp"

namespace edz {

    EResult::EResult() : m_module(0), m_desc(0) {}

    EResult::EResult(u32 module, u32 desc) : m_module(module), m_desc(desc) {}

    EResult::EResult(Result result) : m_module(static_cast<u32>(R_MODULE(result))), m_desc(static_cast<u32>(R_DESCRIPTION(result))) {}

    u32 EResult::getModule() {
        return this->m_module;
    }

    u32 EResult::getDescription() {
        return this->m_desc;
    }

    std::string EResult::getString() {
        return edz::hlp::formatString("2%03d-%04d (0x%x)", this->getModule(), this->getDescription(), static_cast<u32>(*this));
    }

    bool EResult::operator==(EResult &other) {
        return this->getDescription() == other.getDescription();
    }

    bool EResult::operator!=(EResult &other) {
        return this->getDescription() != other.getDescription();
    }

    bool EResult::operator==(Result &other) {
        return this->getDescription() == EResult(other).getDescription() && this->getDescription() == EResult(other).getModule();
    }

    bool EResult::operator!=(Result &other) {
        return this->getDescription() != EResult(other).getDescription() || this->getDescription() != EResult(other).getModule();
    }

    bool EResult::succeeded() {
        return this->m_desc == 0;
    }

    bool EResult::failed() {
        return this->m_desc != 0;
    }

    EResult EResult::operator=(u32 &other) {
        return Result(other);
    }

    EResult EResult::operator=(EResult &other) {
        return EResult(other.getDescription());
    }

    EResult EResult::operator=(Result other) {
        return EResult(static_cast<u32>(other));
    }

    EResult::operator u32() const { 
        return MAKERESULT(this->m_module, this->m_desc);
    }

}