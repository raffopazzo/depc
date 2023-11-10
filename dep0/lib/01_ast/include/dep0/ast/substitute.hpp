#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

template <Properties P>
void substitute(type_t<P>&, typename type_t<P>::var_t const&, type_t<P> const&);

template <Properties P>
void substitute(
    typename type_t<P>::arr_t::arg_iterator begin,
    typename type_t<P>::arr_t::arg_iterator end,
    sort_t<P>& ret_type,
    typename type_t<P>::var_t const&,
    type_t<P> const&);

template <Properties P>
void substitute(
    typename expr_t<P>::abs_t::arg_iterator begin,
    typename expr_t<P>::abs_t::arg_iterator end,
    body_t<P>&,
    typename expr_t<P>::var_t const&,
    expr_t<P> const&);

template <Properties P>
void substitute(expr_t<P>&, typename expr_t<P>::var_t const&, expr_t<P> const&);

} // namespace dep0::ast

#include "dep0/ast/substitute_impl.hpp"
