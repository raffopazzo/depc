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

/**
 * Remove unreachable statements from the given range.
 * A statement is unreachable if no execution path will ever reach it,
 * regardless of the value assumed by all variables.
 * Removal is performed similarly to `std::remove()` and `std::remove_if()`,
 * i.e. unreachable statements are shifted towards the end of the range
 * and the new past-the-end iterator is returned.
 *
 * @return  A boolean indicating whether unreachable statements have been removed,
 *          together with the new past-the-end iterator, which can be passed to std::vector::erase.
 *          If the boolean is false, the iterator returned is the end iterator passed as input.
 *          Note that if the boolean is true, the returned iterator may also be the original end iterator;
 *          for example in the statement `if (true) foo(); else bar();` the else branch can be dropped,
 *          without affecting the end iterator of the original sequence.
 */
std::pair<bool, std::vector<stmt_t>::iterator>
drop_unreachable_stmts(
    std::vector<stmt_t>::iterator,
    std::vector<stmt_t>::iterator);

} // namespace dep0::typecheck
