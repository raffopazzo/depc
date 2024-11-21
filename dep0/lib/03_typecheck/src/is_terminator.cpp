/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/is_terminator.hpp"

#include "private/returns_from_all_branches.hpp"

#include "dep0/match.hpp"

namespace dep0::typecheck {

bool is_terminator(stmt_t const& s)
{
    return match(
        s.value,
        [] (expr_t::app_t const&) { return false; },
        [] (stmt_t::if_else_t const& x) { return returns_from_all_branches(x); },
        [] (stmt_t::return_t const&) { return true; },
        [] (stmt_t::impossible_t const&)
        {
            // Reaching the impossible statement is absurd, so we might as well treat it as a terminator.
            // Ex falso quodlibet.
            return true;
        });
}

} // namespace dep0::typecheck
