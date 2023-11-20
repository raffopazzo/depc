#pragma once

#include "dep0/ast/contains_var.hpp"

#include "dep0/match.hpp"

#include <algorithm>

namespace dep0::ast {

// forward declaration

template <Properties P> bool contains_var(body_t<P> const&, typename expr_t<P>::var_t const&, occurrence_style);
template <Properties P> bool contains_var(typename expr_t<P>::app_t const&, typename expr_t<P>::var_t const&, occurrence_style);

// implementation

template <Properties P>
bool contains_var(expr_t<P> const& x, typename expr_t<P>::var_t const& var, occurrence_style const style)
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
                    return contains_var(x.lhs.get(), var, style) or contains_var(x.rhs.get(), var, style);
                });
        },
        [&] (expr_t<P>::var_t const& x)
        {
            return x == var;
        },
        [&] (expr_t<P>::app_t const& x)
        {
            return contains_var<P>(x, var, style);
        },
        [&] (expr_t<P>::abs_t const& x)
        {
            if (contains_var(x.args.begin(), x.args.end(), x.ret_type.get(), var, style))
                return true;
            if (style == occurrence_style::free)
                return std::ranges::none_of(x.args, [&](auto const& arg) { return arg.var == var; })
                    and contains_var(x.body, var, occurrence_style::free);
            else
                return contains_var(x.body, var, occurrence_style::anywhere);
        },
        [&] (expr_t<P>::pi_t const& x)
        {
            return contains_var(x.args.begin(), x.args.end(), x.ret_type.get(), var, style);
        });
}

template <Properties P>
bool contains_var(typename expr_t<P>::app_t const& x, typename expr_t<P>::var_t const& var, occurrence_style const style)
{
    return std::ranges::any_of(x.args, [&] (expr_t<P> const& arg) { return contains_var(arg, var, style); }) or
        contains_var(x.func.get(), var, style);
}

template <Properties P>
bool contains_var(
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
            if (contains_var(arg.type, var, style))
                return true;
            if (arg.var == var)
                return true;
        }
    }
    else
    {
        for (auto const& arg: std::ranges::subrange(begin, end))
        {
            if (contains_var(arg.type, var, style))
                return true;
            if (arg.var == var)
                return false;
        }
        return contains_var(ret_type, var, style);
    }
    return contains_var(ret_type, var, style);
}

template <Properties P>
bool contains_var(body_t<P> const& x, typename expr_t<P>::var_t const& var, occurrence_style const style)
{
    return std::ranges::any_of(
        x.stmts,
        [&] (stmt_t<P> const& stmt)
        {
            return match(
                stmt.value,
                [&] (expr_t<P>::app_t const& app)
                {
                    return contains_var<P>(app, var, style);
                },
                [&] (stmt_t<P>::if_else_t const& if_)
                {
                    return contains_var(if_.cond, var, style)
                        or contains_var(if_.true_branch, var, style)
                        or if_.false_branch and contains_var(*if_.false_branch, var, style);
                },
                [&] (stmt_t<P>::return_t const& ret)
                {
                    return ret.expr and contains_var(*ret.expr, var, style);
                });
        });
}

} // namespace dep0::ast

