/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Declares `dep0::ast::occurs_in()` and associated overloads.
 */
#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

/**
 * @brief Specifies whether `dep0::ast::occurs_in()` should check for
 * free occurrences or for any occurrences of a variable name.
 * 
 * A variable `x` occurs free if its value is not bound to a function argument named `x`.
 */
enum class occurrence_style
{
    free,
    anywhere
};

/** @brief Returns true if the given variable name appears (free or anywhere) in the given expression. */
template <Properties P>
bool occurs_in(typename expr_t<P>::var_t const&, expr_t<P> const&, occurrence_style);

/** @brief Returns true if the given variable name appears (free or anywhere) in any function argument. */
template <Properties P>
bool occurs_in(
    typename expr_t<P>::var_t const&,
    typename std::vector<func_arg_t<P>>::const_iterator begin,
    typename std::vector<func_arg_t<P>>::const_iterator end,
    occurrence_style);

/**
 * @brief Returns true if the given variable name appears (free or anywhere) in:
 *   - any function argument
 *   - the function return type
 *   - the function body, if one is supplied.
 */
template <Properties P>
bool occurs_in(
    typename expr_t<P>::var_t const&,
    typename std::vector<func_arg_t<P>>::const_iterator begin,
    typename std::vector<func_arg_t<P>>::const_iterator end,
    expr_t<P> const& ret_type,
    body_t<P> const* body,
    occurrence_style);

} // namespace dep0::ast

#include "dep0/ast/occurs_in_impl.hpp"
