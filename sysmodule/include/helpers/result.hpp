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

#include <switch.h>

#define MODULE_EDIZONSYSMODULE 338

namespace edz {
    class EResult {
    public:
        EResult();

        EResult(u32 module, u32 desc);

        EResult(Result result);

        constexpr u32 getModule();

        constexpr u32 getDescription();

        bool operator==(EResult &other);
        bool operator==(Result &other);
        bool operator!=(EResult &other);
        bool operator!=(Result &other);

        EResult operator=(u32 &other);
        EResult operator=(EResult &other);
        EResult operator=(Result other);

        operator u32() const;

        bool succeeded();
        bool failed();

    private:
        const u32 m_module, m_desc;
    };
}