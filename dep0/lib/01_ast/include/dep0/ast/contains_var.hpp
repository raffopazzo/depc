#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

enum class occurrence_style
{
    free,
    anywhere
};

template <Properties P>
bool contains_var(expr_t<P> const&, typename expr_t<P>::var_t const&, occurrence_style);

// can also be used for pi-types
template <Properties P>
bool contains_var(
    typename expr_t<P>::abs_t::arg_const_iterator begin,
    typename expr_t<P>::abs_t::arg_const_iterator end,
    expr_t<P> const& ret_type,
    typename expr_t<P>::var_t const&,
    occurrence_style);

} // namespace dep0::ast

#include "dep0/ast/contains_var_impl.hpp"
