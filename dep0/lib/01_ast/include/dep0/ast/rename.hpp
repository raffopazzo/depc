#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

// can also be used to rename pi-types
template <Properties P>
typename expr_t<P>::var_t rename(
    typename expr_t<P>::var_t const&,
    typename expr_t<P>::abs_t::arg_iterator begin,
    typename expr_t<P>::abs_t::arg_iterator end,
    expr_t<P>& ret_type,
    body_t<P>*);

template <Properties P>
typename expr_t<P>::var_t rename(typename expr_t<P>::var_t const&, body_t<P>&);

} // namespace dep0::ast

#include "dep0/ast/rename_impl.hpp"

