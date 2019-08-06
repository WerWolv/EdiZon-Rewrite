/**
 * Copyright (C) 2019 averne
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

#include <cstdint>
#include <utility>
#include <type_traits>

#include "overlay/utils.hpp"

namespace edz::ovl {

    struct rgb565_t {
        union {
            struct {
                uint16_t r: 5, g: 6, b: 5;
            } __attribute__((packed));
            uint16_t rgb;
        };
        inline rgb565_t(uint16_t raw): rgb(raw) { }
        inline rgb565_t(uint8_t r, uint8_t g, uint8_t b): r(r), g(g), b(b) { }
    };
    ASSERT_SIZE(rgb565_t, 2);
    ASSERT_STANDARD_LAYOUT(rgb565_t);

    struct rgba4444_t {
        union {
            struct {
                uint16_t r: 4, g: 4, b: 4, a: 4;
            } __attribute__((packed));
            uint16_t rgba;
        };
        inline rgba4444_t(uint16_t raw): rgba(raw) { }
        inline rgba4444_t(uint8_t r, uint8_t g, uint8_t b, uint8_t a): r(r), g(g), b(b), a(a) { }
    };
    ASSERT_SIZE(rgba4444_t, 2);
    ASSERT_STANDARD_LAYOUT(rgba4444_t);

    template<typename T, typename=void>
    struct has_alpha : std::false_type { };

    template<typename T>
    struct has_alpha<T, decltype(std::declval<T>().a, void())> : std::true_type { };

    template<typename T>
    inline constexpr bool has_alpha_v = has_alpha<T>::value;

    template<typename T>
    struct col_underlying_type { using type = decltype(T::r); };

    template<typename T>
    using col_underlying_type_t = typename col_underlying_type<T>::type;

    template<typename T>
    inline T makeColor(col_underlying_type_t<T> raw) {
        return T(raw);
    }

    template<typename T>
    typename std::enable_if_t<!has_alpha_v<T>, T>
    inline makeColor(decltype(T::r) r, decltype(T::g) g, decltype(T::b) b) {
        return T(r, g, b);
    }

    template<typename T>
    typename std::enable_if_t<has_alpha_v<T>, T>
    inline makeColor(decltype(T::r) r, decltype(T::g) g, decltype(T::b) b, decltype(T::a) a=-1) {
        return T(r, g, b, a);
    }

    template<typename T>
    typename std::enable_if_t<!has_alpha_v<T>, T>
    inline make_color_min_alpha() {
        return makeColor<T>(0, 0, 0);
    }

    template<typename T>
    typename std::enable_if_t<has_alpha_v<T>, T>
    inline make_color_min_alpha() {
        return makeColor<T>(0, 0, 0, 0);
    }

    template<typename T>
    inline T make_color_max_alpha(decltype(T::r) r, decltype(T::g) g, decltype(T::b) b) {
        return makeColor<T>(r, g, b);
    }

    template<typename T>
    inline T make_color_max_all() {
        return makeColor<T>(-1, -1, -1);
    }

    #define BLEND_CHANNEL(x, y, a) (((a) * (x) + ((0xff - (a)) * (y))) / 0xff)

    template<typename T>
    typename std::enable_if_t<!has_alpha_v<T>, T>
    inline blend(T x, T y, uint8_t alpha) {
        return makeColor<T>(
            BLEND_CHANNEL(x.r, y.r, alpha),
            BLEND_CHANNEL(x.g, y.g, alpha),
            BLEND_CHANNEL(x.b, y.b, alpha)
        );
    }

    template<typename T>
    typename std::enable_if_t<has_alpha_v<T>, T>
    inline blend(T x, T y, uint8_t alpha) {
        return makeColor<T>(
            BLEND_CHANNEL(x.r, y.r, alpha),
            BLEND_CHANNEL(x.g, y.g, alpha),
            BLEND_CHANNEL(x.b, y.b, alpha),
            BLEND_CHANNEL(1,   y.a, alpha)
        );
    }
    
}

