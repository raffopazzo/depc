#pragma once

#include "dep0/typecheck/ast.hpp"

namespace dep0::typecheck {

/**
 * @return true if unreachable statements have been removed from the given body.
 */
bool drop_unreachable_stmts(body_t&);

} // namespace dep0::typecheck
