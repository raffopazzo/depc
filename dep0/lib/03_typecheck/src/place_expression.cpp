/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/place_expression.hpp"

#include "dep0/match.hpp"

namespace dep0::typecheck {

is_place_expression_result_t is_place_expression(expr_t const& expr)
{
    using namespace is_place_expression_result;
    using result_t = is_place_expression_result_t;
    return match(
        expr.value,
        [] (expr_t::because_t const& x) { return is_place_expression(x.value.get()); },
        [] (expr_t::var_t const& x) { return result_t{var_t{x}}; },
        [] (expr_t::deref_t const& x) { return result_t{deref_t{x.expr.get()}}; },
        [] (expr_t::member_t const& x) { return result_t{member_t{x.object.get(), x.field}}; },
        [] (expr_t::subscript_t const& x) { return result_t{subscript_t{x.object.get(), x.index.get()}}; },
        [] (auto const&) { return result_t{no_t{}}; });
}

} // namespace dep0::typecheck
