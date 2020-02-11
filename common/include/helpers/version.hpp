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

#pragma once

#include <edizon.hpp>

namespace edz::hlp {

    class Version { 
    public:
        u8 major, minor, patch;

        constexpr bool operator==(Version other) {
            return this->major == other.major && this->minor == other.minor && this->patch == other.patch;
        }

        constexpr bool operator!=(Version other) {
            return !operator==(other);
        }

        constexpr bool operator>(Version other) {
            return (major << 16 | minor << 8 | patch) > (other.major << 16 | other.minor << 8 | other.patch);
        }

        constexpr bool operator<(Version other) {
            return (major << 16 | minor << 8 | patch) < (other.major << 16 | other.minor << 8 | other.patch);
        }

        constexpr bool operator>=(Version other) {
            return operator>(other) || operator==(other);
        }

        constexpr bool operator<=(Version other) {
            return operator<(other) || operator==(other);
        }

        std::string getString() {
            return std::to_string(this->major) + "." + std::to_string(this->minor) + "." + std::to_string(this->patch);
        }
    };

}