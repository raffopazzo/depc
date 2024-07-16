#pragma once

#include "dep0/typecheck/ast.hpp"

namespace dep0::typecheck {

/**
 * Return true if all branches of execution inside the given body contain impossible statements.
 * If all branches are impossible, the entire body can be removed from the final programme.
 */
bool is_impossible(body_t const&);

} // namespace dep0::typecheck
