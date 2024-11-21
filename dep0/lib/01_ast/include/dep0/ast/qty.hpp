/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include <algorithm>

namespace dep0::ast {

/** Represents the quantity associated to a function argument; default is `many`, unless specified otherwise. */
enum class qty_t
{
    // do not change ordering
    zero = 0,
    one = 1,
    many = 2
};

inline qty_t operator+(qty_t const a, qty_t const b)
{
    auto const x = static_cast<int>(a);
    auto const y = static_cast<int>(b);
    return static_cast<qty_t>(std::min(2, x + y));
}

inline qty_t operator*(qty_t const a, qty_t const b)
{
    auto const x = static_cast<int>(a);
    auto const y = static_cast<int>(b);
    return static_cast<qty_t>(std::min(2, x * y));
}

} // namespace dep0::ast
