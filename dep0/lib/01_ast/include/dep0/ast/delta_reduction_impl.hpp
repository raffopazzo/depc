#pragma once

#include "dep0/ast/delta_reduction.hpp"

#include "dep0/match.hpp"

#include <cassert>
#include <ranges>

namespace dep0::ast {

template <Properties P>
using context_t = delta_reduction::context_t<P>;

template <Properties P> bool delta_reduce(context_t<P> const&, typename stmt_t<P>::if_else_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename stmt_t<P>::return_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::arith_expr_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::boolean_constant_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::numeric_constant_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::var_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::app_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::abs_t&);

template <Properties P>
bool delta_reduce(context_t<P> const& ctx, func_def_t<P>& def)
{
    return delta_reduce<P>(ctx, def.value);
}

template <Properties P>
bool delta_reduce(context_t<P> const& ctx, body_t<P>& body)
{
    for (auto& stmt: body.stmts)
        if (delta_reduce(ctx, stmt))
            return true;
    return false;
}

template <Properties P>
bool delta_reduce(context_t<P> const& ctx, stmt_t<P>& stmt)
{
    return match(stmt.value, [&] (auto& x) { return delta_reduce<P>(ctx, x); });
}

template <Properties P>
bool delta_reduce(context_t<P> const& ctx, typename stmt_t<P>::if_else_t& if_)
{
    return delta_reduce(ctx, if_.cond)
        or delta_reduce(ctx, if_.true_branch)
        or if_.false_branch and delta_reduce(ctx, *if_.false_branch);
}

template <Properties P>
bool delta_reduce(context_t<P> const& ctx, typename stmt_t<P>::return_t& ret)
{
    return ret.expr and delta_reduce(ctx, *ret.expr);
}

template <Properties P>
bool delta_reduce(context_t<P> const& ctx, expr_t<P>& expr)
{
    return match(
        expr.value,
        [&] (typename expr_t<P>::var_t& var)
        {
            if (auto const val = ctx[var.name])
                if (auto const abs = std::get_if<typename expr_t<P>::abs_t>(val))
                {
                    expr.value = *abs;
                    return true;
                }
            return false;
        },
        [&] (auto& x) { return delta_reduce<P>(ctx, x); });
}

template <Properties P>
bool delta_reduce(context_t<P> const& ctx, typename expr_t<P>::arith_expr_t& x)
{
    return match(
        x.value,
        [&] (typename expr_t<P>::arith_expr_t::plus_t& x)
        {
            return delta_reduce(ctx, x.lhs.get()) or delta_reduce(ctx, x.rhs.get());
        });
}

template <Properties P>
bool delta_reduce(context_t<P> const& ctx, typename expr_t<P>::boolean_constant_t&) { return false; }

template <Properties P>
bool delta_reduce(context_t<P> const& ctx, typename expr_t<P>::numeric_constant_t&) { return false; }

template <Properties P>
bool delta_reduce(context_t<P> const& ctx, typename expr_t<P>::app_t& app)
{
    if (delta_reduce(ctx, app.func.get()))
        return true;
    for (auto& arg: app.args)
        if (delta_reduce(ctx, arg))
            return true;
    return false;
}

template <Properties P>
bool delta_reduce(context_t<P> const& ctx, typename expr_t<P>::abs_t& abs)
{
    auto ctx2 = ctx.extend();
    for (auto const& arg: abs.args)
        match(
            arg.value,
            [&] (auto const& arg)
            {
                if (arg.var)
                {
                    bool const inserted = ctx2.try_emplace(arg.var->name, delta_reduction::something_else_t{}).second;
                    assert(inserted);
                }
            });
    return delta_reduce(ctx2, abs.body);
}

template <Properties P>
bool delta_reduce(context_t<P> const& ctx, type_t<P>&) { return false; }

} // namespace dep0::ast
