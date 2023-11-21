#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

enum class occurrence_style
{
    free,
    anywhere
};

template <Properties P>
bool occurs_in(typename expr_t<P>::var_t const&, expr_t<P> const&, occurrence_style);

template <Properties P>
bool occurs_in(
    typename expr_t<P>::var_t const&,
    typename std::vector<func_arg_t<P>>::const_iterator begin,
    typename std::vector<func_arg_t<P>>::const_iterator end,
    expr_t<P> const& ret_type,
    occurrence_style);

} // namespace dep0::ast

#include "dep0/ast/occurs_in_impl.hpp"
