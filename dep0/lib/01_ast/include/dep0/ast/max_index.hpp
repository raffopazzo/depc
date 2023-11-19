#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

template <Properties P>
std::size_t max_index(func_arg_t<P> const&);

template <Properties P>
std::size_t max_index(body_t<P> const&);

template <Properties P>
std::size_t max_index(expr_t<P> const&);

template <Properties P>
std::size_t max_index(typename expr_t<P>::app_t const&);

} // namespace dep0::ast

#include "dep0/ast/max_index_impl.hpp"
