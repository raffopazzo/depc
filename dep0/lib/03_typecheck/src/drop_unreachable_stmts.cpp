#include "private/drop_unreachable_stmts.hpp"

#include "private/returns_from_all_branches.hpp"

namespace dep0::typecheck {

bool drop_unreachable_stmts(body_t& body)
{
    auto const is_return = [] (stmt_t const& x)
    {
        return std::holds_alternative<stmt_t::return_t>(x.value);
    };
    bool changed = false;
    // 1. all statements after the first return are unreachable;
    if (auto const it = std::ranges::find_if(body.stmts, is_return); it != body.stmts.end())
    {
        auto const old_size = body.stmts.size();
        body.stmts.erase(std::next(it), body.stmts.end());
        changed |= old_size != body.stmts.size();
    }
    // 1bis. similarly to (1) all statements after an if-else that returns from all branches are also unreachable;
    if (auto const it =
            std::ranges::find_if(
                body.stmts,
                [] (stmt_t const& x)
                {
                    auto const if_else = std::get_if<stmt_t::if_else_t>(&x.value);
                    return if_else and returns_from_all_branches(*if_else);
                });
        it != body.stmts.end())
    {
        auto const old_size = body.stmts.size();
        body.stmts.erase(std::next(it), body.stmts.end());
        changed |= old_size != body.stmts.size();
    }
    // 2. drop unreachable if-else bodies, i.e. bodies guarded by a boolean constant of the wrong value
    for (auto& stmt: body.stmts)
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
    for (auto& stmt: body.stmts)
        if (auto* const if_else = std::get_if<stmt_t::if_else_t>(&stmt.value))
        {
            changed |= drop_unreachable_stmts(if_else->true_branch);
            if (if_else->false_branch)
                changed |= drop_unreachable_stmts(*if_else->false_branch);
        }
    return changed;
}

} // namespace dep0::typecheck
