#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

template <Properties P>
typename type_t<P>::var_t rename(
    typename type_t<P>::var_t const&,
    typename type_t<P>::arr_t::arg_iterator begin,
    typename type_t<P>::arr_t::arg_iterator end,
    sort_t<P>& ret_type);

template <Properties P>
typename expr_t<P>::var_t rename(typename expr_t<P>::var_t const&, body_t<P>&);

} // namespace dep0::ast

#include "dep0/ast/rename_impl.hpp"

