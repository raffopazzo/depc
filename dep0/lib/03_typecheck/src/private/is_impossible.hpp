#pragma once

#include "dep0/typecheck/ast.hpp"

namespace dep0::typecheck {

/**
 * Return true if the given statement is (or otherwise contains) the impossible statement.
 * For example `return absurd(t, proof_of_false);` contains the impossible statement.
 */
bool is_impossible(stmt_t const&);

} // namespace dep0::typecheck
