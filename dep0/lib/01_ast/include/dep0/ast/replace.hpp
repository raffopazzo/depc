#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

// can also be used to replace inside pi-types
template <Properties P>
void replace(
    typename expr_t<P>::var_t const&,
    typename expr_t<P>::var_t const&,
    typename expr_t<P>::abs_t::arg_iterator begin,
    typename expr_t<P>::abs_t::arg_iterator end,
    expr_t<P>& ret_type);

template <Properties P>
void replace(typename expr_t<P>::var_t const&, typename expr_t<P>::var_t const&, body_t<P>&);

template <Properties P>
void replace(typename expr_t<P>::var_t const&, typename expr_t<P>::var_t const&, expr_t<P>&);

template <Properties P>
void replace(typename expr_t<P>::var_t const&, typename expr_t<P>::var_t const&, typename expr_t<P>::app_t&);

} // namespace dep0::ast

#include "dep0/ast/replace_impl.hpp"
