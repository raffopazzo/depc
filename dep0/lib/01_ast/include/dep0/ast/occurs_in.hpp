#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

enum class occurrence_style
{
    free,
    anywhere
};

/** @return true if the given variable name appears (free or anywhere) in the given expression. */
template <Properties P>
bool occurs_in(typename expr_t<P>::var_t const&, expr_t<P> const&, occurrence_style);

/** @return true if the given variable name appears (free or anywhere) in the type of any function argumet. */
template <Properties P>
bool occurs_in(
    typename expr_t<P>::var_t const&,
    typename std::vector<func_arg_t<P>>::const_iterator begin,
    typename std::vector<func_arg_t<P>>::const_iterator end,
    occurrence_style);

/**
 * @return true if the given variable name appears (either free or anywhere) in:
 *   - the type of any function argumet
 *   - the function return type
 *   - the function body, if one is supplied.
 */
template <Properties P>
bool occurs_in(
    typename expr_t<P>::var_t const&,
    typename std::vector<func_arg_t<P>>::const_iterator begin,
    typename std::vector<func_arg_t<P>>::const_iterator end,
    expr_t<P> const& ret_type,
    body_t<P> const* body,
    occurrence_style);

} // namespace dep0::ast

#include "dep0/ast/occurs_in_impl.hpp"
