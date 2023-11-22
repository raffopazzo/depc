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
