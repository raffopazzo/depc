#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

template <Properties P>
bool contains_var(type_t<P> const&, typename type_t<P>::var_t const&);

template <Properties P>
bool contains_var(type_t<P> const&, typename expr_t<P>::var_t const&);

template <Properties P>
bool contains_var(func_arg_t<P> const&, typename type_t<P>::var_t const&);

template <Properties P>
bool contains_var(func_arg_t<P> const&, typename expr_t<P>::var_t const&);

template <Properties P>
bool contains_var(body_t<P> const&, typename expr_t<P>::var_t const&);

template <Properties P>
bool contains_var(expr_t<P> const&, typename expr_t<P>::var_t const&);

template <Properties P>
bool contains_var(typename expr_t<P>::app_t const&, typename expr_t<P>::var_t const&);

} // namespace dep0::ast

#include "dep0/ast/contains_var_impl.hpp"
