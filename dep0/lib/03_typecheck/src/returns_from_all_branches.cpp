#include "private/returns_from_all_branches.hpp"

namespace dep0::typecheck {

bool returns_from_all_branches(body_t const& body)
{
    struct visitor
    {
        bool operator()(expr_t::app_t const&) const { return false; }
        bool operator()(stmt_t::if_else_t const& x) const { return returns_from_all_branches(x); }
        bool operator()(stmt_t::return_t const&) const { return true; }
    };
    // a top level return statement shadows any other statement that may follow it; so can immediately return true;
    // similary, an if-else that returns from both branches shadows any statement that may follow it;
    // so, in either case, we can immediately return true as soon as either is true
    for (auto const& s: body.stmts)
        if (std::visit(visitor{}, s.value))
            return true;
    return false;
}

bool returns_from_all_branches(stmt_t::if_else_t const& x)
{
    return x.false_branch and returns_from_all_branches(x.true_branch) and returns_from_all_branches(*x.false_branch);
}

} // namespace dep0::typecheck
