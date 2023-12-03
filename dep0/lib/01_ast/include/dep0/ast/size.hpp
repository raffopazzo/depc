#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

template <Properties P> std::size_t size(expr_t<P> const&);

} // namespace dep0::ast

#include "dep0/ast/size_impl.hpp"
