#pragma once

#include "dep0/ast/beta_reduction.hpp"
#include "dep0/ast/substitute.hpp"

#include "dep0/match.hpp"

#include <algorithm>
#include <cassert>
#include <ranges>

namespace dep0::ast {

namespace impl {

template <typename T>
void destructive_self_assign(T& x, T&& y)
{
    auto tmp = std::move(y);
    x = std::move(tmp);
}

template <Properties P> bool beta_normalize(typename stmt_t<P>::if_else_t&);
template <Properties P> bool beta_normalize(typename stmt_t<P>::return_t&);
template <Properties P> bool beta_normalize(typename expr_t<P>::typename_t&);
template <Properties P> bool beta_normalize(typename expr_t<P>::bool_t&);
template <Properties P> bool beta_normalize(typename expr_t<P>::unit_t&);
template <Properties P> bool beta_normalize(typename expr_t<P>::i8_t&);
template <Properties P> bool beta_normalize(typename expr_t<P>::i16_t&);
template <Properties P> bool beta_normalize(typename expr_t<P>::i32_t&);
template <Properties P> bool beta_normalize(typename expr_t<P>::i64_t&);
template <Properties P> bool beta_normalize(typename expr_t<P>::u8_t&);
template <Properties P> bool beta_normalize(typename expr_t<P>::u16_t&);
template <Properties P> bool beta_normalize(typename expr_t<P>::u32_t&);
template <Properties P> bool beta_normalize(typename expr_t<P>::u64_t&);
template <Properties P> bool beta_normalize(typename expr_t<P>::boolean_constant_t&);
template <Properties P> bool beta_normalize(typename expr_t<P>::numeric_constant_t&);
template <Properties P> bool beta_normalize(typename expr_t<P>::arith_expr_t&);
template <Properties P> bool beta_normalize(typename expr_t<P>::var_t&);
template <Properties P> bool beta_normalize(typename expr_t<P>::app_t&);
template <Properties P> bool beta_normalize(typename expr_t<P>::abs_t&);
template <Properties P> bool beta_normalize(typename expr_t<P>::pi_t&);

template <Properties P>
bool beta_normalize(typename stmt_t<P>::if_else_t& if_)
{
    bool changed = beta_normalize(if_.cond);
    changed |= beta_normalize(if_.true_branch);
    if (if_.false_branch)
        changed |= beta_normalize(*if_.false_branch);
    return changed;
}

template <Properties P>
bool beta_normalize(typename stmt_t<P>::return_t& ret)
{
    return ret.expr and beta_normalize(*ret.expr);
}

template <Properties P> bool beta_normalize(typename expr_t<P>::typename_t&) { return false; }
template <Properties P> bool beta_normalize(typename expr_t<P>::bool_t&) { return false; }
template <Properties P> bool beta_normalize(typename expr_t<P>::unit_t&) { return false; }
template <Properties P> bool beta_normalize(typename expr_t<P>::i8_t&) { return false; }
template <Properties P> bool beta_normalize(typename expr_t<P>::i16_t&) { return false; }
template <Properties P> bool beta_normalize(typename expr_t<P>::i32_t&) { return false; }
template <Properties P> bool beta_normalize(typename expr_t<P>::i64_t&) { return false; }
template <Properties P> bool beta_normalize(typename expr_t<P>::u8_t&) { return false; }
template <Properties P> bool beta_normalize(typename expr_t<P>::u16_t&) { return false; }
template <Properties P> bool beta_normalize(typename expr_t<P>::u32_t&) { return false; }
template <Properties P> bool beta_normalize(typename expr_t<P>::u64_t&) { return false; }
template <Properties P> bool beta_normalize(typename expr_t<P>::boolean_constant_t&) { return false; }
template <Properties P> bool beta_normalize(typename expr_t<P>::numeric_constant_t&) { return false; }

template <Properties P>
bool beta_normalize(typename expr_t<P>::arith_expr_t& x)
{
    // TODO constant folding?
    return match(
        x.value,
        [&] (typename expr_t<P>::arith_expr_t::plus_t& x)
        {
            bool changed = beta_normalize(x.lhs.get());
            changed |= beta_normalize(x.rhs.get());
            return changed;
        });
}

template <Properties P> bool beta_normalize(typename expr_t<P>::var_t&) { return false; }

template <Properties P>
bool beta_normalize(typename expr_t<P>::app_t& app)
{
    // Technically, this classifies as applicative-order-reduction;
    // as such, if the abstraction discards some/all arguments we might waste time reducing those arguments.
    // Currently it doesn't matter; we can reassess in future.
    bool changed = beta_normalize(app.func.get());
    for (auto& arg: app.args)
        changed |= beta_normalize(arg);
    // TODO could also be application of pi_t
    if (auto* const abs = std::get_if<typename expr_t<P>::abs_t>(&app.func.get().value))
    {
        if (abs->args.size() > 0ul)
        {
            if (abs->args.size() != app.args.size()) // always false when beta-normalizing legal terms
                return changed;
            for (auto const i: std::views::iota(0ul, abs->args.size()))
            {
                auto const& arg = abs->args[i];
                if (arg.var)
                    substitute(
                        *arg.var,
                        app.args[i],
                        abs->args.begin() + i + 1,
                        abs->args.end(),
                        abs->ret_type.get(),
                        &abs->body);
            }
            // at this point all arguments of the abstraction have been substituted,
            // so we can remove them and normalize the new body
            app.args.clear();
            abs->args.clear();
            changed = true;
            beta_normalize(abs->body);
        }
    }
    return changed;
}

template <Properties P>
bool beta_normalize(typename expr_t<P>::abs_t& abs)
{
    bool changed = false;
    for (auto& arg: abs.args)
        changed |= beta_normalize(arg.type);
    changed |= beta_normalize(abs.ret_type.get());
    changed |= beta_normalize(abs.body);
    return changed;
}

template <Properties P>
bool beta_normalize(typename expr_t<P>::pi_t& pi)
{
    bool changed = false;
    for (auto& arg: pi.args)
        changed |= beta_normalize(arg.type);
    changed |= beta_normalize(pi.ret_type.get());
    return changed;
}

} // namespace impl
template <Properties P>
bool beta_normalize(module_t<P>& m)
{
    bool changed = false;
    for (auto& def: m.func_defs)
        changed |= beta_normalize(def);
    return changed;
}

template <Properties P>
bool beta_normalize(func_def_t<P>& def)
{
    return impl::beta_normalize<P>(def.value);
}

template <Properties P>
bool beta_normalize(body_t<P>& body)
{
    auto const is_return = [] (stmt_t<P> const& x)
    {
        return std::holds_alternative<typename stmt_t<P>::return_t>(x.value);
    };
    // NB taking `stmts` by value because we are about to perform a destructive self-assignment
    auto replace_with = [&] (std::vector<stmt_t<P>>::iterator it, std::vector<stmt_t<P>> stmts)
    {
        // If there is nothing to replace `it` with, then just remove it;
        // otherwise we need to splice all the replacing statements in place of `it`,
        // which can be done efficiently by replacing `it` with the first statement, and insert the rest after it.
        // Finally, if the new statements contain a return statment, any other statement after that can be dropped.
        if (stmts.empty())
            return body.stmts.erase(it);
        auto const index_of_ret = [&]
        {
            auto const j = std::ranges::find_if(stmts, is_return);
            return j == stmts.end() ? std::nullopt : std::optional{std::distance(stmts.begin(), j)};
        }();
        auto j = stmts.begin();
        *it++ = std::move(*j++);
        it = body.stmts.insert(it, std::make_move_iterator(j), std::make_move_iterator(stmts.end()));
        if (index_of_ret)
            // if you were expecting a `+1` here, the reason it's missing is because we manually incremented `it` once
            body.stmts.erase(it + *index_of_ret, body.stmts.end());
        return it;
    };
    bool changed = false;
    auto it = body.stmts.begin();
    while (it != body.stmts.end())
    {
        changed |= beta_normalize(*it);
        it = match(
            it->value,
            [&] (typename expr_t<P>::app_t& app)
            {
                // if immediately-invoked lambda we can extract the body,
                // but have to suppress return statements from the invoked lambda,
                // otherwise they would become early returns from the parent function
                // TODO blindly suppressing return of the invoked lambda will be wrong once we have side-effects
                if (app.args.empty())
                    if (auto* const abs = std::get_if<typename expr_t<P>::abs_t>(&app.func.get().value))
                    {
                        changed = true;
                        // TODO fix this: if body contains 2 return statements, all others in between must be dropped
                        std::erase_if(abs->body.stmts, is_return);
                        return replace_with(it, std::move(abs->body.stmts));
                    }
                return std::next(it);
            },
            [&] (typename stmt_t<P>::if_else_t& if_)
            {
                if (auto const c = std::get_if<typename expr_t<P>::boolean_constant_t>(&if_.cond.value))
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
            [&] (typename stmt_t<P>::return_t const&) { return std::next(it); });
    }
    return changed;
}

template <Properties P>
bool beta_normalize(stmt_t<P>& stmt)
{
    return match(stmt.value, [&] (auto& x) { return impl::beta_normalize<P>(x); });
}

template <Properties P>
bool beta_normalize(expr_t<P>& expr)
{
    return match(
        expr.value,
        [&] (typename expr_t<P>::app_t& app)
        {
            bool changed = impl::beta_normalize<P>(app);
            // if this reduced to a parameterless single return statement, we can extract the returned expression
            if (app.args.empty())
                if (auto* const abs = std::get_if<typename expr_t<P>::abs_t>(&app.func.get().value))
                    if (abs->body.stmts.size() == 1ul)
                        if (auto* const ret = std::get_if<typename stmt_t<P>::return_t>(&abs->body.stmts[0].value))
                            if (ret->expr)
                            {
                                changed = true;
                                impl::destructive_self_assign(expr, std::move(*ret->expr));
                            }
            return changed;
        },
        [&] (auto& x) { return impl::beta_normalize<P>(x); });
}

} // namespace dep0::ast

