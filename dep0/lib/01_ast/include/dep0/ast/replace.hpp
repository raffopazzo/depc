/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Declares @ref `dep0::ast::replace()` and its overloads.
 */
#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

/**
 * @brief Replaces a variable name with another one.
 *
 * This function is used when renaming variables during alpha-conversion and
 * is intended to be invoked on Pi-Types and Lambda-Abstractions.
 *
 * @remarks calling this function `rename` would be incorrect because
 * in the context of alpha-conversion renaming means something else.
 * It means to **replace**, hence the name of this function, all
 * occurrences of a binding variable with a new "fresh" name in order to
 * obtain a new expression that is alpha-equivalent to the original one.
 * 
 * @warning for the purpose of alpha-conversion, it is the responsibility
 * of the caller to ensure that the new name is actually fresh.
 * If the new name is not fresh then accidental capture will occur.
 * 
 * @see
 *   - @ref alpha_equivalence
 *   - @ref dep0::ast::rename()
 *
 * @param from The old variable name to replace.
 * @param to The new variable name to replace it with.
 */
template <Properties P>
void replace(
    typename expr_t<P>::var_t const& from,
    typename expr_t<P>::var_t const& to,
    typename std::vector<func_arg_t<P>>::iterator begin,
    typename std::vector<func_arg_t<P>>::iterator end,
    expr_t<P>& ret_type,
    body_t<P>*);

/** @brief Overload to use for Sigma-Types. */
template <Properties P>
void replace(
    typename expr_t<P>::var_t const& from,
    typename expr_t<P>::var_t const& to,
    typename std::vector<func_arg_t<P>>::iterator begin,
    typename std::vector<func_arg_t<P>>::iterator end);

} // namespace dep0::ast

#include "dep0/ast/replace_impl.hpp"
