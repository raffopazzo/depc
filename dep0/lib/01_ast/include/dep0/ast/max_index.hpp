/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Single-function header declaring `dep0::ast::max_index()`.
 */
#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

/**
 * @brief Return the highest of all @ref dep0::ast::expr_t::var_t::idx values that
 * appears in all given function arguments, return type and body (unless `nullptr`).
 *
 * This function is used when renaming variables during alpha-conversion and
 * is intended to be invoked on Pi-Types and Lambda-Abstractions.
 *
 * @see
 *   - @ref alpha_equivalence
 *   - @ref dep0::ast::rename()
 */
template <Properties P>
std::size_t max_index(
    typename std::vector<func_arg_t<P>>::const_iterator begin,
    typename std::vector<func_arg_t<P>>::const_iterator end,
    expr_t<P> const& ret_type,
    body_t<P> const* body);

} // namespace dep0::ast

#include "dep0/ast/max_index_impl.hpp"
