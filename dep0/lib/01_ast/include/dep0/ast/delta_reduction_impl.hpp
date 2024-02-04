#pragma once

#include "dep0/ast/delta_reduction.hpp"

#include "dep0/match.hpp"

#include <cassert>
#include <ranges>

namespace dep0::ast {

namespace impl {

template <Properties P>
using context_t = delta_reduction::context_t<P>;

template <Properties P> bool delta_reduce(context_t<P> const&, typename stmt_t<P>::if_else_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename stmt_t<P>::return_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::typename_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::bool_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::unit_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::i8_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::i16_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::i32_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::i64_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::u8_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::u16_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::u32_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::u64_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::boolean_constant_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::numeric_constant_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::boolean_expr_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::relation_expr_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::arith_expr_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::var_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::app_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::pi_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::array_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::init_list_t&);
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::subscript_t&);

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

template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::typename_t&) { return false; }
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::bool_t&) { return false; }
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::unit_t&) { return false; }
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::i8_t&) { return false; }
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::i16_t&) { return false; }
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::i32_t&) { return false; }
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::i64_t&) { return false; }
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::u8_t&) { return false; }
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::u16_t&) { return false; }
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::u32_t&) { return false; }
template <Properties P> bool delta_reduce(context_t<P> const&, typename expr_t<P>::u64_t&) { return false; }

template <Properties P>
bool delta_reduce(context_t<P> const&, typename expr_t<P>::boolean_constant_t&) { return false; }

template <Properties P>
bool delta_reduce(context_t<P> const&, typename expr_t<P>::numeric_constant_t&) { return false; }

template <Properties P>
bool delta_reduce(context_t<P> const& ctx, typename expr_t<P>::boolean_expr_t& x)
{
    return match(
        x.value,
        [&] (typename expr_t<P>::boolean_expr_t::not_t& x)
        {
            return delta_reduce(ctx, x.expr.get());
        },
        [&] (auto& x)
        {
            return delta_reduce(ctx, x.lhs.get()) or delta_reduce(ctx, x.rhs.get());
        });
}

template <Properties P>
bool delta_reduce(context_t<P> const& ctx, typename expr_t<P>::relation_expr_t& x)
{
    return delta_reduce(ctx, x.lhs.get()) or delta_reduce(ctx, x.rhs.get());
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
bool delta_reduce(context_t<P> const&, typename expr_t<P>::var_t&)
{
    // We only want to perform a "simplied" one-step delta-reduction,
    // so ignore variable experssions that appear on their own.
    return false;
}

template <Properties P>
bool delta_reduce(context_t<P> const& ctx, typename expr_t<P>::app_t& app)
{
    if (auto const var = std::get_if<typename expr_t<P>::var_t>(&app.func.get().value))
        if (auto const abs = std::get_if<typename expr_t<P>::abs_t>(ctx[*var]))
        {
            app.func.get().value = *abs;
            return true;
        }
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
    for (auto& arg: abs.args)
    {
        if (delta_reduce(ctx2, arg.type))
            return true;
        if (arg.var)
        {
            bool const inserted = ctx2.try_emplace(*arg.var, delta_reduction::something_else_t{}).second;
            assert(inserted);
        }
    }
    if (delta_reduce(ctx2, abs.ret_type.get()))
        return true;
    return delta_reduce(ctx2, abs.body);
}

template <Properties P>
bool delta_reduce(context_t<P> const& ctx, typename expr_t<P>::pi_t& pi)
{
    auto ctx2 = ctx.extend();
    for (auto& arg: pi.args)
    {
        if (delta_reduce(ctx2, arg.type))
            return true;
        if (arg.var)
        {
            bool const inserted = ctx2.try_emplace(*arg.var, delta_reduction::something_else_t{}).second;
            assert(inserted);
        }
    }
    return delta_reduce(ctx2, pi.ret_type.get());
}

template <Properties P>
bool delta_reduce(context_t<P> const&, typename expr_t<P>::array_t&)
{
    return false;
}

template <Properties P>
bool delta_reduce(context_t<P> const& ctx, typename expr_t<P>::init_list_t& init_list)
{
    for (auto& v: init_list.values)
        if (delta_reduce(ctx, v))
            return true;
    return false;
}

template <Properties P>
bool delta_reduce(context_t<P> const& ctx, typename expr_t<P>::subscript_t& subscript)
{
    return delta_reduce(ctx, subscript.array.get()) or delta_reduce(ctx, subscript.index.get());
}

} // namespace impl

template <Properties P>
bool delta_reduce(delta_reduction::context_t<P> const& ctx, func_def_t<P>& def)
{
    return impl::delta_reduce<P>(ctx, def.value);
}

template <Properties P>
bool delta_reduce(delta_reduction::context_t<P> const& ctx, body_t<P>& body)
{
    for (auto& stmt: body.stmts)
        if (delta_reduce(ctx, stmt))
            return true;
    return false;
}

template <Properties P>
bool delta_reduce(delta_reduction::context_t<P> const& ctx, stmt_t<P>& stmt)
{
    return match(stmt.value, [&] (auto& x) { return impl::delta_reduce<P>(ctx, x); });
}

template <Properties P>
bool delta_reduce(delta_reduction::context_t<P> const& ctx, expr_t<P>& expr)
{
    return match(expr.value, [&] (auto& x) { return impl::delta_reduce<P>(ctx, x); });
}

} // namespace dep0::ast
