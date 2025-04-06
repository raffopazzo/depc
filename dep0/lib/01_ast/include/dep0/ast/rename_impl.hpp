/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Implements `dep0::ast::rename()`
 */
#pragma once

#include "dep0/ast/rename.hpp"

#include "dep0/ast/max_index.hpp"
#include "dep0/ast/replace.hpp"

#include <algorithm>

namespace dep0::ast {

template <Properties P>
typename expr_t<P>::var_t rename(
    typename expr_t<P>::var_t const& var,
    typename std::vector<func_arg_t<P>>::iterator const begin,
    typename std::vector<func_arg_t<P>>::iterator const end,
    expr_t<P>& ret_type,
    body_t<P>* body)
{
    auto const new_idx = 1ul + std::max(var.idx, max_index(begin, end, ret_type, body));
    auto const new_var = typename expr_t<P>::var_t{var.name, new_idx};
    replace(var, new_var, begin, end, ret_type, body);
    return new_var;
}

template <Properties P>
typename expr_t<P>::var_t rename(
    typename expr_t<P>::var_t const& var,
    typename std::vector<func_arg_t<P>>::iterator const begin,
    typename std::vector<func_arg_t<P>>::iterator const end)
{
    auto const new_idx = 1ul + std::max(var.idx, max_index<P>(begin, end));
    auto const new_var = typename expr_t<P>::var_t{var.name, new_idx};
    replace<P>(var, new_var, begin, end);
    return new_var;
}

} // namespace dep0::ast
