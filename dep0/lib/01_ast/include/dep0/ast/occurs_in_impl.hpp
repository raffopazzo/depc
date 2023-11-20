#pragma once

#include "dep0/ast/occurs_in.hpp"

#include "dep0/match.hpp"

#include <algorithm>

namespace dep0::ast {

// forward declaration

template <Properties P> bool occurs_in(body_t<P> const&, typename expr_t<P>::var_t const&, occurrence_style);
template <Properties P> bool occurs_in(typename expr_t<P>::app_t const&, typename expr_t<P>::var_t const&, occurrence_style);

// implementation

template <Properties P>
bool occurs_in(expr_t<P> const& x, typename expr_t<P>::var_t const& var, occurrence_style const style)
{
    return match(
        x.value,
        [&] (expr_t<P>::typename_t const&) { return false; },
        [&] (expr_t<P>::bool_t const&) { return false; },
        [&] (expr_t<P>::unit_t const&) { return false; },
        [&] (expr_t<P>::i8_t const&) { return false; },
        [&] (expr_t<P>::i16_t const&) { return false; },
        [&] (expr_t<P>::i32_t const&) { return false; },
        [&] (expr_t<P>::i64_t const&) { return false; },
        [&] (expr_t<P>::u8_t const&) { return false; },
        [&] (expr_t<P>::u16_t const&) { return false; },
        [&] (expr_t<P>::u32_t const&) { return false; },
        [&] (expr_t<P>::u64_t const&) { return false; },
        [&] (expr_t<P>::boolean_constant_t const&) { return false; },
        [&] (expr_t<P>::numeric_constant_t const&) { return false; },
        [&] (expr_t<P>::arith_expr_t const& x)
        {
            return match(
                x.value,
                [&] (expr_t<P>::arith_expr_t::plus_t const& x)
                {
                    return occurs_in(x.lhs.get(), var, style) or occurs_in(x.rhs.get(), var, style);
                });
        },
        [&] (expr_t<P>::var_t const& x)
        {
            return x == var;
        },
        [&] (expr_t<P>::app_t const& x)
        {
            return occurs_in<P>(x, var, style);
        },
        [&] (expr_t<P>::abs_t const& x)
        {
            if (occurs_in(x.args.begin(), x.args.end(), x.ret_type.get(), var, style))
                return true;
            if (style == occurrence_style::free)
                return std::ranges::none_of(x.args, [&](auto const& arg) { return arg.var == var; })
                    and occurs_in(x.body, var, occurrence_style::free);
            else
                return occurs_in(x.body, var, occurrence_style::anywhere);
        },
        [&] (expr_t<P>::pi_t const& x)
        {
            return occurs_in(x.args.begin(), x.args.end(), x.ret_type.get(), var, style);
        });
}

template <Properties P>
bool occurs_in(typename expr_t<P>::app_t const& x, typename expr_t<P>::var_t const& var, occurrence_style const style)
{
    return std::ranges::any_of(x.args, [&] (expr_t<P> const& arg) { return occurs_in(arg, var, style); }) or
        occurs_in(x.func.get(), var, style);
}

template <Properties P>
bool occurs_in(
    typename expr_t<P>::abs_t::arg_const_iterator begin,
    typename expr_t<P>::abs_t::arg_const_iterator end,
    expr_t<P> const& ret_type,
    typename expr_t<P>::var_t const& var,
    occurrence_style const style)
{
    if (style == occurrence_style::anywhere)
    {
        for (auto const& arg: std::ranges::subrange(begin, end))
        {
            if (occurs_in(arg.type, var, style))
                return true;
            if (arg.var == var)
                return true;
        }
    }
    else
    {
        for (auto const& arg: std::ranges::subrange(begin, end))
        {
            if (occurs_in(arg.type, var, style))
                return true;
            if (arg.var == var)
                return false;
        }
        return occurs_in(ret_type, var, style);
    }
    return occurs_in(ret_type, var, style);
}

template <Properties P>
bool occurs_in(body_t<P> const& x, typename expr_t<P>::var_t const& var, occurrence_style const style)
{
    return std::ranges::any_of(
        x.stmts,
        [&] (stmt_t<P> const& stmt)
        {
            return match(
                stmt.value,
                [&] (expr_t<P>::app_t const& app)
                {
                    return occurs_in<P>(app, var, style);
                },
                [&] (stmt_t<P>::if_else_t const& if_)
                {
                    return occurs_in(if_.cond, var, style)
                        or occurs_in(if_.true_branch, var, style)
                        or if_.false_branch and occurs_in(*if_.false_branch, var, style);
                },
                [&] (stmt_t<P>::return_t const& ret)
                {
                    return ret.expr and occurs_in(*ret.expr, var, style);
                });
        });
}

} // namespace dep0::ast

