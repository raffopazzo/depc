/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "dep0/digit_separator.hpp"

namespace dep0 {

bool contains_digit_separator(std::string_view const in)
{
    return in.find('\'') != std::string_view::npos;
}

void remove_digit_separator(std::string_view const in, std::string& out)
{
    for (char const c: in)
        if (c != '\'')
            out.push_back(c);
}

std::string remove_digit_separator(std::string_view const in)
{
    std::string out;
    out.reserve(in.size());
    remove_digit_separator(in, out);
    return out;
}

} // namespace dep0

