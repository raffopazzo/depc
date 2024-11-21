/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/proof_state.hpp"

#include "private/rewrite.hpp"

namespace dep0::typecheck {

proof_state_t::proof_state_t(ctx_t c, sort_t s) :
    context(std::move(c)),
    goal(std::move(s))
{
}

void proof_state_t::rewrite(expr_t const& from, expr_t const& to)
{
    context = context.rewrite(from, to);
    // TODO could have a mutable-ref version instead (or as well)
    if (auto new_goal = typecheck::rewrite(from, to, goal))
        goal = std::move(*new_goal);
}

} // namespace dep0::typecheck

