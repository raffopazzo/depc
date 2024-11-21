/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "dep0/testing/ostream.hpp"

namespace dep0::ast {

std::ostream& operator<<(std::ostream& os, is_mutable_t const m)
{
    return os << (m == is_mutable_t::yes ? "yes" : "no");
}

} // namespace dep0::ast
