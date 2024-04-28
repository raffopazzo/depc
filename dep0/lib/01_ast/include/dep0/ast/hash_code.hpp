#pragma once

#include "dep0/ast/ast.hpp"

namespace dep0::ast {

/**
 * Compute the hash code of the given expression.
 * Alpha-equivalent expressions are guaranteeed to have the same hash code.
 */
template <Properties P> std::size_t hash_code(expr_t<P> const&);

} // namespace dep0::ast

#include "dep0/ast/hash_code_impl.hpp"
