#pragma once

#include "dep0/ast/rename.hpp"

#include "dep0/ast/max_index.hpp"
#include "dep0/ast/replace.hpp"

#include <numeric>

namespace dep0::ast {

template <Properties P>
typename expr_t<P>::var_t rename(
    typename expr_t<P>::var_t const& var,
    typename expr_t<P>::abs_t::arg_iterator const begin,
    typename expr_t<P>::abs_t::arg_iterator const end,
    expr_t<P>& ret_type,
    body_t<P>* body)
{
    auto const max_idx =
        std::accumulate(
            begin, end,
            std::max(var.name.idx, std::max(max_index(ret_type), body ? max_index(*body) : 0ul)),
            [] (std::size_t const acc, func_arg_t<P> const& arg)
            {
                return std::max(acc, std::max(max_index(arg.type), arg.var ? arg.var->name.idx : 0ul));
            });
    auto const new_var = typename expr_t<P>::var_t{ast::indexed_var_t{var.name.txt, max_idx + 1ul}};
    replace(var, new_var, begin, end, ret_type);
    if (body)
        replace(var, new_var, *body);
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
