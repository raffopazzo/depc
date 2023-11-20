#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

enum class occurrence_style
{
    free,
    anywhere
};

template <Properties P>
bool occurs_in(expr_t<P> const&, typename expr_t<P>::var_t const&, occurrence_style);

template <Properties P>
bool occurs_in(
    typename std::vector<func_arg_t<P>>::const_iterator begin,
    typename std::vector<func_arg_t<P>>::const_iterator end,
    expr_t<P> const& ret_type,
    typename expr_t<P>::var_t const&,
    occurrence_style);

} // namespace dep0::ast

#include "dep0/ast/occurs_in_impl.hpp"
