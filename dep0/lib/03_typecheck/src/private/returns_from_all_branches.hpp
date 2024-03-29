#pragma once

#include "dep0/typecheck/ast.hpp"

namespace dep0::typecheck {

/**
 * Checks whether the given body contains an explicit return statement from every branch.
 *
 * @return True if all branches contain an explicit return statement, false otherwise.
 */
bool returns_from_all_branches(body_t const&);

/**
 * @return  True if the input if-else statement contains both branches and
 *          both of them contain an explicit return statement.
 */
bool returns_from_all_branches(stmt_t::if_else_t const&);

} // namespace dep0::typecheck
