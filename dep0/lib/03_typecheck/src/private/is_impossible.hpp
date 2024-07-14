#pragma once

#include "dep0/typecheck/ast.hpp"

namespace dep0::typecheck {

/**
 * Return true if all branches of execution inside given body contain impossible statements.
 * If all branches are impossible, the entire body can be removed from the final programme.
 */
bool is_impossible(body_t const&);

/**
 * Return true if the given statement is (or otherwise contains) the impossible statement.
 * For example `return absurd(t, proof_of_false);` contains the impossible statement.
 */
bool is_impossible(stmt_t const&);

} // namespace dep0::typecheck
