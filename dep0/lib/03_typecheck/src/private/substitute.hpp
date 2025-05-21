/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Functions that perform @ref substitution.
 */
#pragma once

#include "dep0/typecheck/ast.hpp"

namespace dep0::typecheck {

/**
 * @brief Perform capture-avoiding substitution inside Pi-Types and Lambda-Abstractions.
 *
 * @param var The name of the variable to substitute.
 * @param expr The expression that should be substituted for `var`.
 * @param begin Perform substitution inside all function arguments in the range `[begin, end)`.
 * @param end Ditto.
 * @param ret_type Perform substitution inside the return type of the Pi-Type/Lambda-Abstraction.
 * @param body If not `nullptr`, perform substitution inside the body of a Lambda-Abstraction.
 */
void substitute(
    expr_t::var_t const& var,
    expr_t const& expr,
    std::vector<func_arg_t>::iterator begin,
    std::vector<func_arg_t>::iterator end,
    expr_t& ret_type,
    body_t* body);

/** @brief Overload to use for Sigma-Types. */
void substitute(
    expr_t::var_t const& var,
    expr_t const& expr,
    std::vector<func_arg_t>::iterator begin,
    std::vector<func_arg_t>::iterator end);

/** @brief Perform capture-avoiding substitution inside all fields of a struct. */
void substitute(
    expr_t::var_t const&,
    expr_t const&,
    std::vector<type_def_t::struct_t::field_t>::iterator begin,
    std::vector<type_def_t::struct_t::field_t>::iterator end);

} // namespace dep0::typecheck
