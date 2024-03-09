#pragma once

#include "dep0/typecheck/ast.hpp"

namespace dep0::typecheck {

/**
 * Remove unreachable statements from the given body.
 * A statement is unreachable if no execution path will ever reach it,
 * regardless of the value assumed by all variables.
 *
 * @return  True if unreachable statements have been removed;
 *          false if all statements are reachable.
 */
bool drop_unreachable_stmts(body_t&);

} // namespace dep0::typecheck
