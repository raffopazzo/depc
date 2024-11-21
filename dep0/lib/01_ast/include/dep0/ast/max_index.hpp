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
std::size_t max_index(
    typename std::vector<func_arg_t<P>>::const_iterator begin,
    typename std::vector<func_arg_t<P>>::const_iterator end,
    expr_t<P> const& ret_type,
    body_t<P> const* body);

} // namespace dep0::ast

#include "dep0/ast/max_index_impl.hpp"
