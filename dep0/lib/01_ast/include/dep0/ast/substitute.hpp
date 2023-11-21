#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

template <Properties P>
void substitute(
    typename expr_t<P>::var_t const&,
    expr_t<P> const&,
    typename std::vector<func_arg_t<P>>::iterator begin,
    typename std::vector<func_arg_t<P>>::iterator end,
    expr_t<P>& ret_type);

template <Properties P>
void substitute(typename expr_t<P>::var_t const&, expr_t<P> const&, body_t<P>&);

} // namespace dep0::ast

#include "dep0/ast/substitute_impl.hpp"
