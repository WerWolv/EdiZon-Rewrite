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

namespace edz::hlp {

    class Version { 
    public:
        u8 major, minor, patch;

        bool operator==(Version other) {
            return this->major == other.major && this->minor == other.minor && this->patch == other.patch;
        }

        bool operator!=(Version other) {
            return !operator==(other);
        }

        bool operator>(Version other) {
            if (this->major > other.major)
                return true;
            else if (this->minor > other.minor)
                return true;
            else if (this->patch > other.patch)
                return true;
            else return false;
        }

        bool operator<(Version other) {
            if (this->major < other.major)
                return true;
            else if (this->minor < other.minor)
                return true;
            else if (this->patch < other.patch)
                return true;
            else return false;
        }

        bool operator>=(Version other) {
            return operator>(other) || operator==(other);
        }

        bool operator<=(Version other) {
            return operator<(other) || operator==(other);
        }
    };

}