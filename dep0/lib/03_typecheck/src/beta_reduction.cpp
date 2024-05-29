#include "private/beta_reduction.hpp"

#include "private/drop_unreachable_stmts.hpp"
#include "private/is_mutable.hpp"
#include "private/substitute.hpp"

#include "dep0/destructive_self_assign.hpp"
#include "dep0/match.hpp"
#include "dep0/vector_splice.hpp"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <optional>
#include <ranges>

namespace dep0::typecheck {

namespace impl {

static bool beta_normalize(stmt_t&);
static bool beta_normalize(stmt_t::if_else_t&);
static bool beta_normalize(stmt_t::return_t&);

static bool beta_normalize(expr_t::typename_t&) { return false; }
static bool beta_normalize(expr_t::true_t&) { return false; }
static bool beta_normalize(expr_t::auto_t&) { return false; }
static bool beta_normalize(expr_t::bool_t&) { return false; }
static bool beta_normalize(expr_t::cstr_t&) { return false; }
static bool beta_normalize(expr_t::unit_t&) { return false; }
static bool beta_normalize(expr_t::i8_t&) { return false; }
static bool beta_normalize(expr_t::i16_t&) { return false; }
static bool beta_normalize(expr_t::i32_t&) { return false; }
static bool beta_normalize(expr_t::i64_t&) { return false; }
static bool beta_normalize(expr_t::u8_t&) { return false; }
static bool beta_normalize(expr_t::u16_t&) { return false; }
static bool beta_normalize(expr_t::u32_t&) { return false; }
static bool beta_normalize(expr_t::u64_t&) { return false; }
static bool beta_normalize(expr_t::boolean_constant_t&) { return false; }
static bool beta_normalize(expr_t::numeric_constant_t&) { return false; }
static bool beta_normalize(expr_t::string_literal_t&) { return false; }
static bool beta_normalize(expr_t::boolean_expr_t&);
static bool beta_normalize(expr_t::relation_expr_t&);
static bool beta_normalize(expr_t::arith_expr_t&);
static bool beta_normalize(expr_t::var_t&) { return false; }
static bool beta_normalize(expr_t::global_t&) { return false; }
static bool beta_normalize(expr_t::app_t&);
static bool beta_normalize(expr_t::abs_t&);
static bool beta_normalize(expr_t::pi_t&);
static bool beta_normalize(expr_t::array_t&) { return false; }
static bool beta_normalize(expr_t::init_list_t&);
static bool beta_normalize(expr_t::subscript_t&);

bool beta_normalize(stmt_t& stmt)
{
    return match(stmt.value, [&] (auto& x) { return beta_normalize(x); });
}

bool beta_normalize(stmt_t::if_else_t& if_)
{
    bool changed = beta_normalize(if_.cond);
    changed |= beta_normalize(if_.true_branch);
    if (if_.false_branch)
        changed |= beta_normalize(*if_.false_branch);
    return changed;
}

bool beta_normalize(stmt_t::return_t& ret)
{
    return ret.expr and beta_normalize(*ret.expr);
}

bool beta_normalize(expr_t::boolean_expr_t& x)
{
    return match(
        x.value,
        [] (expr_t::boolean_expr_t::not_t& x) { return beta_normalize(x.expr.get()); },
        [] (auto& x) -> bool { return beta_normalize(x.lhs.get()) | beta_normalize(x.rhs.get()); });
}

bool beta_normalize(expr_t::relation_expr_t& x)
{
    return match(x.value, [&] (auto& x) -> bool { return beta_normalize(x.lhs.get()) | beta_normalize(x.rhs.get()); });
}

bool beta_normalize(expr_t::arith_expr_t& x)
{
    return match(x.value, [&] (auto& x) -> bool { return beta_normalize(x.lhs.get()) | beta_normalize(x.rhs.get()); });
}

bool beta_normalize(expr_t::app_t& app)
{
    // Technically, this classifies as applicative-order-reduction;
    // as such, if the abstraction discards some/all arguments we might waste time reducing those arguments.
    // Currently it doesn't matter; we can reassess in future.
    bool changed = beta_normalize(app.func.get());
    for (auto& arg: app.args)
        changed |= beta_normalize(arg);
    if (std::ranges::any_of(app.args, [] (expr_t const& arg) { return is_mutable(arg); }))
        // We cannot push any argument inside the body if it is mutable,
        // otherwise we might alter the original programme if the argument is not used exactly once;
        // in principle we could try to prove that the argument is used exactly once,
        // but for now that's extra complexity that we don't need; we can reassess in future.
        return changed;
    if (auto* const abs = std::get_if<expr_t::abs_t>(&app.func.get().value))
    {
        if (abs->args.size() > 0ul)
        {
            assert(abs->args.size() == app.args.size()); // always true for legal terms
            for (auto const i: std::views::iota(0ul, abs->args.size()))
                if (auto const& arg = abs->args[i]; arg.var)
                    substitute(
                        *arg.var,
                        app.args[i],
                        abs->args.begin() + i + 1,
                        abs->args.end(),
                        abs->ret_type.get(),
                        &abs->body);
            // at this point all arguments of the abstraction have been substituted,
            // so we can remove them and normalize the new body;
            // also note that now the function type becomes a pi-type with no arguments
            // TODO we should also perform substitution inside the function type but we need a test first
            std::get<expr_t::pi_t>(std::get<expr_t>(app.func.get().properties.sort.get()).value).args.clear();
            app.args.clear();
            abs->args.clear();
            changed = true;
            beta_normalize(abs->body);
        }
    }
    return changed;
}

bool beta_normalize(expr_t::abs_t& abs)
{
    bool changed = false;
    for (auto& arg: abs.args)
        changed |= beta_normalize(arg.type);
    changed |= beta_normalize(abs.ret_type.get());
    changed |= beta_normalize(abs.body);
    return changed;
}

bool beta_normalize(expr_t::pi_t& pi)
{
    bool changed = false;
    for (auto& arg: pi.args)
        changed |= beta_normalize(arg.type);
    changed |= beta_normalize(pi.ret_type.get());
    return changed;
}

bool beta_normalize(expr_t::init_list_t& init_list)
{
    bool changed = false;
    for (auto& v: init_list.values)
        changed |= beta_normalize(v);
    return changed;
}

bool beta_normalize(expr_t::subscript_t& subscript)
{
    return beta_normalize(subscript.array.get()) | beta_normalize(subscript.index.get());
}

} // namespace impl

bool beta_normalize(body_t& body)
{
    // NB taking `stmts` by value because we are about to perform a destructive self-assignment
    auto const replace_with = [&] (std::vector<stmt_t>::iterator it, std::vector<stmt_t> stmts)
    {
        // replace `it` with the new statements and then drop whatever has now become unreachable because of them
        it = vector_splice(body.stmts, it, std::move(stmts));
        auto const new_end = drop_unreachable_stmts(it, body.stmts.end()).second;
        body.stmts.erase(new_end, body.stmts.end());
        return it; // the first statement is obviously reachable, so `it` is still valid
    };
    bool changed = drop_unreachable_stmts(body);
    auto it = body.stmts.begin();
    while (it != body.stmts.end())
    {
        changed |= impl::beta_normalize(*it);
        it = match(
            it->value,
            [&] (expr_t::app_t const& app)
            {
                // calls to immutable functions with immutable arguments can be removed since they do nothing;
                // keep anything else
                return is_mutable(app) ? std::next(it) : body.stmts.erase(it);
            },
            [&] (stmt_t::if_else_t& if_)
            {
                // this step is arguably iota-reduction
                if (auto const c = std::get_if<expr_t::boolean_constant_t>(&if_.cond.value))
                {
                    changed = true;
                    if (c->value)
                        return replace_with(it, std::move(if_.true_branch.stmts));
                    else if (if_.false_branch)
                        return replace_with(it, std::move(if_.false_branch->stmts));
                    else
                        return body.stmts.erase(it);
                }
                else
                    return std::next(it);
            },
            [&] (stmt_t::return_t const&) { return std::next(it); });
    }
    return changed;
}

bool beta_normalize(expr_t& expr)
{
    bool changed = false;
    if (auto const type = std::get_if<expr_t>(&expr.properties.sort.get()))
        changed |= beta_normalize(*type);
    match(
        expr.value,
        [&] (expr_t::app_t& app)
        {
            changed |= impl::beta_normalize(app);
            // if this reduced to a parameterless single return statement, we can extract the returned expression
            if (app.args.empty())
                if (auto* const abs = std::get_if<expr_t::abs_t>(&app.func.get().value))
                    if (abs->body.stmts.size() == 1ul)
                        if (auto* const ret = std::get_if<stmt_t::return_t>(&abs->body.stmts[0].value))
                            if (ret->expr)
                            {
                                changed = true;
                                destructive_self_assign(expr.value, std::move(ret->expr->value));
                            }
        },
        [&] (auto& x) { changed |= impl::beta_normalize(x); });
    return changed;
}

} // namespace dep0::typecheck
