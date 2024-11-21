/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include <string>
#include <string_view>

namespace dep0 {

bool contains_digit_separator(std::string_view);
void remove_digit_separator(std::string_view, std::string&);
std::string remove_digit_separator(std::string_view);

} // namespace dep0
