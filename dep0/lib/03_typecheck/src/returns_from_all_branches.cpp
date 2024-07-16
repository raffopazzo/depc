#include "private/returns_from_all_branches.hpp"

#include "private/is_terminator.hpp"

#include <algorithm>

namespace dep0::typecheck {

bool returns_from_all_branches(body_t const& body)
{
    return std::ranges::any_of(body.stmts, [] (stmt_t const& s) { return is_terminator(s); });
}

bool returns_from_all_branches(stmt_t::if_else_t const& x)
{
    return x.false_branch and returns_from_all_branches(x.true_branch) and returns_from_all_branches(*x.false_branch);
}

} // namespace dep0::typecheck
