/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Defines `dep0::ast::sign_t`.
 */
#pragma once

namespace dep0::ast {

/** @brief Represents the keywords `signed` or `unsigned` used inside a user-defined integral type definition. */
enum class sign_t
{
    signed_v,
    unsigned_v
};

bool operator<(sign_t, sign_t) = delete;
bool operator<=(sign_t, sign_t) = delete;
bool operator>=(sign_t, sign_t) = delete;
bool operator>(sign_t, sign_t) = delete;

} // namespace dep0::ast
