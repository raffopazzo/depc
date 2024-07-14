#include "private/drop_unreachable_stmts.hpp"

#include "private/returns_from_all_branches.hpp"

#include <algorithm>
#include <ranges>

namespace dep0::typecheck {

bool drop_unreachable_stmts(body_t& body)
{
    auto const [changed, new_end] = drop_unreachable_stmts(body.stmts.begin(), body.stmts.end());
    body.stmts.erase(new_end, body.stmts.end());
    return changed;
}

std::pair<bool, std::vector<stmt_t>::iterator>
drop_unreachable_stmts(
    std::vector<stmt_t>::iterator const begin, // the first statement is always reachable
    std::vector<stmt_t>::iterator end)
{
    // TODO drop everything that contains the impossible statement
    bool changed = false;
    // 1. all statements after the first return are unreachable;
    if (auto const it =
            std::find_if(
                begin, end,
                [] (stmt_t const& x)
                {
                    return std::holds_alternative<stmt_t::return_t>(x.value);
                });
        it != end and std::next(it) != end)
    {
        changed = true;
        end = std::next(it);
    }
    // 1bis. similarly to (1) all statements after an if-else that returns from all branches are also unreachable;
    if (auto const it =
            std::find_if(
                begin, end,
                [] (stmt_t const& x)
                {
                    auto const if_else = std::get_if<stmt_t::if_else_t>(&x.value);
                    return if_else and returns_from_all_branches(*if_else);
                });
        it != end and std::next(it) != end)
    {
        changed = true;
        end = std::next(it);
    }
    // 2. drop unreachable if-else bodies, i.e. bodies guarded by a boolean constant of the opposite value
    for (auto& stmt: std::ranges::subrange(begin, end))
        if (auto* const if_else = std::get_if<stmt_t::if_else_t>(&stmt.value))
            if (auto const c = std::get_if<expr_t::boolean_constant_t>(&if_else->cond.value))
            {
                if (c->value)
                {
                    if (if_else->false_branch)
                    {
                        if_else->false_branch.reset();
                        changed = true;
                    }
                }
                else
                {
                    if (not if_else->true_branch.stmts.empty())
                    {
                        if_else->true_branch.stmts.clear();
                        changed = true;
                    }
                }
            }
    // 3. drop unreachable statements within if-else bodies;
    for (auto& stmt: std::ranges::subrange(begin, end))
        if (auto* const if_else = std::get_if<stmt_t::if_else_t>(&stmt.value))
        {
            changed |= drop_unreachable_stmts(if_else->true_branch);
            if (if_else->false_branch)
                changed |= drop_unreachable_stmts(*if_else->false_branch);
        }
    return std::pair{changed, end};
}

} // namespace dep0::typecheck
