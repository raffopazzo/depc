/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Defines `dep0::ast::qty_t`.
 */
#pragma once

#include <algorithm>

namespace dep0::ast {

/** @brief Represents the quantity associated to a function argument; default is `many`, unless specified otherwise. */
enum class qty_t
{
    // do not change ordering
    zero = 0,
    one = 1,
    many = 2
};

/** @brief Add two quantities together where `1 + 1 = many` and `many + x = many`. */
inline qty_t operator+(qty_t const a, qty_t const b)
{
    auto const x = static_cast<int>(a);
    auto const y = static_cast<int>(b);
    return static_cast<qty_t>(std::min(2, x + y));
}

/** @brief Multiplies two quantities together where `many * many = many`. */
inline qty_t operator*(qty_t const a, qty_t const b)
{
    auto const x = static_cast<int>(a);
    auto const y = static_cast<int>(b);
    return static_cast<qty_t>(std::min(2, x * y));
}

} // namespace dep0::ast
