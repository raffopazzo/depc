/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Functions to manipulate numeric literals containing digit separators, eg `1'000`.
 */
#pragma once

#include <string>
#include <string_view>

namespace dep0 {

/** @brief Retrns true if the input string contains the digit separator character `'`. */
bool contains_digit_separator(std::string_view);

/**
 * @brief Copy all characters from the input view into the output argument, except digit separators.
 *
 * If the input view does not contain a separator, this function will simply write a copy.
 */
void remove_digit_separator(std::string_view, std::string&);

/**
 * @brief Return a copy of the input view, except all digit separators are removed.
 *
 * If the input view does not contain a separator, this function will simply return a copy.
 */
std::string remove_digit_separator(std::string_view);

} // namespace dep0
