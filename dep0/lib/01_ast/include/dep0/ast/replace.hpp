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
