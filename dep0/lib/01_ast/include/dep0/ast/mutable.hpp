/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Defines `dep0::ast::is_mutable_t`.
 */
#pragma once

namespace dep0::ast {

/** @brief Strongly typed boolean to track whether functions, variables and references are mutable or not. */
enum class is_mutable_t
{
    no,
    yes
};

bool operator<(is_mutable_t, is_mutable_t) = delete;
bool operator<=(is_mutable_t, is_mutable_t) = delete;
bool operator>=(is_mutable_t, is_mutable_t) = delete;
bool operator>(is_mutable_t, is_mutable_t) = delete;

} // namespace dep0::ast
