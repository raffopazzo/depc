#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

// TODO API should be consistent with replace() taking const& first and mutable& after

template <Properties P>
void substitute(
    typename std::vector<func_arg_t<P>>::iterator begin,
    typename std::vector<func_arg_t<P>>::iterator end,
    expr_t<P>& ret_type,
    typename expr_t<P>::var_t const&,
    expr_t<P> const&);

template <Properties P>
void substitute(body_t<P>&, typename expr_t<P>::var_t const&, expr_t<P> const&);

template <Properties P>
void substitute(expr_t<P>&, typename expr_t<P>::var_t const&, expr_t<P> const&);

} // namespace dep0::ast

#include "dep0/ast/substitute_impl.hpp"
