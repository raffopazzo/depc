/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Single-function header declaring `dep0::ast::is_place_expression()`.
 */
#pragma once

#include "dep0/ast/ast.hpp"
#include "dep0/ast/unwrap_because.hpp"

#include "dep0/match.hpp"

#include <variant>

namespace dep0::ast {

/** @brief The expression passed to `is_place_expression()` is a value expression. */
struct value_expression_t { };

template <Properties P>
using is_place_expression_result_t =
    std::variant<
        value_expression_t,
        std::reference_wrapper<typename expr_t<P>::var_t const>,
        std::reference_wrapper<typename expr_t<P>::deref_t const>,
        std::reference_wrapper<typename expr_t<P>::member_t const>,
        std::reference_wrapper<typename expr_t<P>::subscript_t const>>;

/**
 * @brief Decide whether the given expression is a place expression or value expression,
 * possibly by looking inside because-expressions, eg `x because reason`.
 *
 * A place expression is an expression that yields a memory location,
 * for example referring to a variable name or the element of an array.
 * Conversely expression that return values are called value expressions.
 *
 * @remarks This concept is inspired by Rust place and value expressions.
 */
template <Properties P>
is_place_expression_result_t<P> is_place_expression(expr_t<P> const& expr)
{
    using result_t = is_place_expression_result_t<P>;
    return match(
        unwrap_because(expr).value,
        [] (expr_t<P>::var_t const& x) { return result_t{std::cref(x)}; },
        [] (expr_t<P>::deref_t const& x) { return result_t{std::cref(x)}; },
        [] (expr_t<P>::member_t const& x) { return result_t{std::cref(x)}; },
        [] (expr_t<P>::subscript_t const& x) { return result_t{std::cref(x)}; },
        [] (auto const&) { return result_t{value_expression_t{}}; });
}

} // namespace dep0::ast
