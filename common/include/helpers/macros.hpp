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

#define ER_TRY(x) { if (EResult res = x; ::edz::EResult(res).failed()) return res; }
#define ER_TRY_RESULT(x, result) { if (EResult res = x; ::edz::EResult(res).failed()) return result; }

#define ER_ASSERT(x) { if (EResult res = x; ::edz::EResult(res).failed()) fatalThrow(res); }
#define ER_ASSERT_RESULT(x, result) { if (EResult res = x; ::edz::EResult(res).failed()) fatalThrow(result); }

#define UNLIKELY(x) __builtin_expect(x, 0)
#define LIKELY(x)   __builtin_expect(x, 1)

#define PRINT_DEBUG(x, ...) printf("[DEBUG] %s:%d | " x "\n", __PRETTY_FUNCTION__, __LINE__ __VA_OPT__(,) __VA_ARGS__)

#define EMPTY_RESPONSE { }