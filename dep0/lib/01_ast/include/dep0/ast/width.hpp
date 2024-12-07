/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Defines `dep0::ast::width_t`.
 */
#pragma once

namespace dep0::ast {

/** @brief Represents the bit width used inside a user-defined integral type definition. */
enum class width_t
{
    _8,
    _16,
    _32,
    _64
};

bool operator<(width_t, width_t) = delete;
bool operator<=(width_t, width_t) = delete;
bool operator>=(width_t, width_t) = delete;
bool operator>(width_t, width_t) = delete;

} // namespace dep0::ast
