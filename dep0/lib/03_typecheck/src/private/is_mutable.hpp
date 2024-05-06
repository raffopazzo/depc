#pragma once

#include "dep0/typecheck/ast.hpp"

namespace dep0::typecheck {

/**
 * Return true if the given expression contains mutable operations, for example call to mutable functions.
 * In other words, if multiple evaluations of this expression may result in different values.
 */
bool is_mutable(expr_t const&);

} // namespace dep0::typecheck
