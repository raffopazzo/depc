/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Single-function header declaring `dep0::typecheck::is_place_expression()`.
 */
#pragma once

#include "dep0/typecheck/ast.hpp"

#include <variant>

namespace dep0::typecheck {

/** @brief Contains the possible results returned from `is_place_expression()`. */
namespace is_place_expression_result
{
    /** @brief The expression passed to `is_place_expression()` is a value expression. */
    struct no_t{};

    /** @brief The expression passed to `is_place_expression()` is a variable. */
    struct var_t
    {
        expr_t::var_t const& var;
    };

    /** @brief The expression passed to `is_place_expression()` is a dereference. */
    struct deref_t
    {
        expr_t const& expr;
    };

    /** @brief The expression passed to `is_place_expression()` is a member field access. */
    struct member_t
    {
        expr_t const& object;
        source_text field;
    };

    /** @brief The expression passed to `is_place_expression()` is a tuple/array access. */
    struct subscript_t
    {
        expr_t const& object;
        expr_t const& index;
    };
};

using is_place_expression_result_t =
    std::variant<
        is_place_expression_result::no_t,
        is_place_expression_result::var_t,
        is_place_expression_result::deref_t,
        is_place_expression_result::member_t,
        is_place_expression_result::subscript_t>;

/**
 * @brief Decide whether the given expression is a place expression or value expression.
 *
 * A place expression is an expression that yields a memory location,
 * for example referring to a variable name or the element of an array.
 * Conversely expression that return values are called value expressions.
 *
 * @remarks This concept is inspired by Rust place and value expressions.
 */
is_place_expression_result_t is_place_expression(expr_t const&);

} // namespace dep0::typecheck
