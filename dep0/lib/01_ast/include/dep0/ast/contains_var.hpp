#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

template <Properties P>
bool contains_var(func_arg_t<P> const&, typename expr_t<P>::var_t const&);

template <Properties P>
bool contains_var(body_t<P> const&, typename expr_t<P>::var_t const&);

template <Properties P>
bool contains_var(expr_t<P> const&, typename expr_t<P>::var_t const&);

template <Properties P>
bool contains_var(typename expr_t<P>::app_t const&, typename expr_t<P>::var_t const&);

// can also be used for pi-types
template <Properties P>
bool contains_var(
    typename expr_t<P>::abs_t::arg_const_iterator begin,
    typename expr_t<P>::abs_t::arg_const_iterator end,
    expr_t<P> const& ret_type,
    typename expr_t<P>::var_t const&);

} // namespace dep0::ast

#include "dep0/ast/contains_var_impl.hpp"
