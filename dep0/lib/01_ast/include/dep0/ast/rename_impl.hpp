#pragma once

#include "dep0/ast/rename.hpp"

#include "dep0/ast/max_index.hpp"
#include "dep0/ast/replace.hpp"

namespace dep0::ast {

template <Properties P>
typename type_t<P>::var_t rename(
    typename type_t<P>::var_t const& var,
    typename type_t<P>::arr_t::arg_iterator const begin,
    typename type_t<P>::arr_t::arg_iterator const end,
    type_t<P>& ret_type)
{
    auto const max_idx = std::max(var.name.idx, max_index(begin, end, ret_type));
    auto const new_var = typename type_t<P>::var_t{ast::indexed_var_t{var.name.txt, max_idx + 1ul}};
    replace(var, new_var, begin, end, ret_type);
    return new_var;
}

template <Properties P>
typename expr_t<P>::var_t rename(typename expr_t<P>::var_t const& var, body_t<P>& body)
{
    auto const max_idx = std::max(var.name.idx, max_index(body));
    auto const new_var = typename expr_t<P>::var_t{ast::indexed_var_t{var.name.txt, max_idx + 1ul}};
    replace(var, new_var, body);
    return new_var;
}

} // namespace dep0::ast
