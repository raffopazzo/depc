#pragma once

#include "dep0/ast/occurs_in.hpp"

#include "dep0/match.hpp"

#include <algorithm>

namespace dep0::ast {

namespace impl {

template <Properties P>
bool occurs_in(typename expr_t<P>::var_t const&, body_t<P> const&, occurrence_style);

template <Properties P>
bool occurs_in(typename expr_t<P>::var_t const&, typename expr_t<P>::app_t const&, occurrence_style);

template <Properties P>
bool occurs_in(typename expr_t<P>::var_t const& var, body_t<P> const& x, occurrence_style const style)
{
    return std::ranges::any_of(
        x.stmts,
        [&] (stmt_t<P> const& stmt)
        {
            return match(
                stmt.value,
                [&] (expr_t<P>::app_t const& app)
                {
                    return occurs_in<P>(var, app, style);
                },
                [&] (stmt_t<P>::if_else_t const& if_)
                {
                    return occurs_in(var, if_.cond, style)
                        or occurs_in(var, if_.true_branch, style)
                        or if_.false_branch and occurs_in(var, *if_.false_branch, style);
                },
                [&] (stmt_t<P>::return_t const& ret)
                {
                    return ret.expr and occurs_in(var, *ret.expr, style);
                });
        });
}

template <Properties P>
bool occurs_in(typename expr_t<P>::var_t const& var, typename expr_t<P>::app_t const& x, occurrence_style const style)
{
    return std::ranges::any_of(x.args, [&] (expr_t<P> const& arg) { return occurs_in(var, arg, style); }) or
        occurs_in(var, x.func.get(), style);
}

} // namespace impl

template <Properties P>
bool occurs_in(typename expr_t<P>::var_t const& var, expr_t<P> const& x, occurrence_style const style)
{
    return match(
        x.value,
        [] (expr_t<P>::typename_t const&) { return false; },
        [] (expr_t<P>::bool_t const&) { return false; },
        [] (expr_t<P>::unit_t const&) { return false; },
        [] (expr_t<P>::i8_t const&) { return false; },
        [] (expr_t<P>::i16_t const&) { return false; },
        [] (expr_t<P>::i32_t const&) { return false; },
        [] (expr_t<P>::i64_t const&) { return false; },
        [] (expr_t<P>::u8_t const&) { return false; },
        [] (expr_t<P>::u16_t const&) { return false; },
        [] (expr_t<P>::u32_t const&) { return false; },
        [] (expr_t<P>::u64_t const&) { return false; },
        [] (expr_t<P>::boolean_constant_t const&) { return false; },
        [] (expr_t<P>::numeric_constant_t const&) { return false; },
        [&] (expr_t<P>::boolean_expr_t const& x)
        {
            return match(
                x.value,
                [&] (expr_t<P>::boolean_expr_t::negation_t const& x)
                {
                    return occurs_in(var, x.expr.get(), style);
                });
        },
        [&] (expr_t<P>::relation_expr_t const& x)
        {
            return occurs_in(var, x.lhs.get(), style) or occurs_in(var, x.rhs.get(), style);
        },
        [&] (expr_t<P>::arith_expr_t const& x)
        {
            return match(
                x.value,
                [&] (expr_t<P>::arith_expr_t::plus_t const& x)
                {
                    return occurs_in(var, x.lhs.get(), style) or occurs_in(var, x.rhs.get(), style);
                });
        },
        [&] (expr_t<P>::var_t const& x)
        {
            return x == var;
        },
        [&] (expr_t<P>::app_t const& x)
        {
            return impl::occurs_in<P>(var, x, style);
        },
        [&] (expr_t<P>::abs_t const& x)
        {
            return occurs_in(var, x.args.begin(), x.args.end(), x.ret_type.get(), &x.body, style);
        },
        [&] (expr_t<P>::pi_t const& x)
        {
            return occurs_in<P>(var, x.args.begin(), x.args.end(), x.ret_type.get(), nullptr, style);
        },
        [] (expr_t<P>::array_t const&)
        {
            return false;
        },
        [&] (expr_t<P>::init_list_t const& x)
        {
            return std::ranges::any_of(x.values, [&] (expr_t<P> const& v) { return occurs_in(var, v, style); });
        },
        [&] (expr_t<P>::subscript_t const& x)
        {
            return occurs_in(var, x.array.get(), style) or occurs_in(var, x.index.get(), style);
        });
}

template <Properties P>
bool occurs_in(
    typename expr_t<P>::var_t const& var,
    typename std::vector<func_arg_t<P>>::const_iterator begin,
    typename std::vector<func_arg_t<P>>::const_iterator end,
    expr_t<P> const& ret_type,
    body_t<P> const* body,
    occurrence_style const style)
{
    for (auto const& arg: std::ranges::subrange(begin, end))
    {
        if (occurs_in(var, arg.type, style))
            return true;
        if (arg.var == var)
            // If we are looking for occurrences anywhere then this is a valid occurrence; return true.
            // If we are looking for free occurrences only then we can immediately return false,
            // since any later occurrence is now bound to this argument.
            return style == occurrence_style::anywhere;
    }
    if (occurs_in(var, ret_type, style))
        return true;
    return body and impl::occurs_in(var, *body, style);
}

} // namespace dep0::ast

