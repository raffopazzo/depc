/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

template <Properties P>
void replace(
    typename expr_t<P>::var_t const&,
    typename expr_t<P>::var_t const&,
    typename std::vector<func_arg_t<P>>::iterator begin,
    typename std::vector<func_arg_t<P>>::iterator end,
    expr_t<P>& ret_type,
    body_t<P>*);

} // namespace dep0::ast

#include "dep0/ast/replace_impl.hpp"
