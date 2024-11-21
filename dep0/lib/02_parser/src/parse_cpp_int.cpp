/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/parse_cpp_int.hpp"

#include "dep0/digit_separator.hpp"

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>

#include <sstream>

namespace dep0::parser {

boost::multiprecision::cpp_int parse_cpp_int(std::string_view txt)
{
    boost::multiprecision::cpp_int value;
    while (txt.starts_with('0'))
        txt.remove_prefix(1);
    if (dep0::contains_digit_separator(txt))
    {
        auto s = std::istringstream(dep0::remove_digit_separator(txt));
        s >> value;
    }
    else
    {
        auto s = boost::iostreams::stream<boost::iostreams::basic_array_source<char>>(txt.begin(), txt.size());
        s >> value;
    }
    return value;
}

} // namespace dep0::parser
