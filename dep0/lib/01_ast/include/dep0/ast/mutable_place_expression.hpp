/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Single-function header declaring `dep0::ast::is_mutable_place_expression()`.
 */
#pragma once

#include "dep0/ast/ast.hpp"
#include "dep0/ast/unwrap_because.hpp"

#include "dep0/match.hpp"

#include <variant>

namespace dep0::ast {

/** @brief The expression passed to `is_mutable_place_expression()` is not a mutable place. */
struct immutable_place_t { };

template <Properties P>
using is_mutable_place_expression_result_t =
    std::variant<
        immutable_place_t,
        std::reference_wrapper<typename expr_t<P>::var_t const>,
        std::reference_wrapper<typename expr_t<P>::member_t const>,
        std::reference_wrapper<typename expr_t<P>::subscript_t const>>;

/**
 * @brief Decide whether or not the given expression is a mutable place expression,
 * possibly by looking inside because-expressions, eg `x because reason`.
 *
 * A mutable place expression is an expression that yields a **possibly** mutable memory location,
 * for example the name of a variable or the element of an array.
 * Whether or not such place is really mutable depends on whether the root variable was declared mutable.
 *
 * @remarks This concept is fundamentally the same as a place expression but without dereferences.
 */
template <Properties P>
is_mutable_place_expression_result_t<P> is_mutable_place_expression(expr_t<P> const& expr)
{
    using result_t = is_mutable_place_expression_result_t<P>;
    return match(
        unwrap_because(expr).value,
        [] (expr_t<P>::var_t const& x) { return result_t{std::cref(x)}; },
        [] (expr_t<P>::member_t const& x) { return result_t{std::cref(x)}; },
        [] (expr_t<P>::subscript_t const& x) { return result_t{std::cref(x)}; },
        [] (auto const&) { return result_t{immutable_place_t{}}; });
}

} // namespace dep0::ast
