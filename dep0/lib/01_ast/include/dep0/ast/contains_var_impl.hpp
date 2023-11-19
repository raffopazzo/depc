#pragma once

#include "dep0/ast/contains_var.hpp"

#include "dep0/match.hpp"

#include <algorithm>

namespace dep0::ast {

template <Properties P>
bool contains_var(func_arg_t<P> const& arg, typename expr_t<P>::var_t const& var)
{
    return contains_var(arg.type, var) or arg.var == var;
}

template <Properties P>
bool contains_var(body_t<P> const& x, typename expr_t<P>::var_t const& var)
{
    return std::ranges::any_of(
        x.stmts,
        [&] (stmt_t<P> const& stmt)
        {
            return match(
                stmt.value,
                [&] (typename expr_t<P>::app_t const& app)
                {
                    return contains_var<P>(app, var);
                },
                [&] (typename stmt_t<P>::if_else_t const& if_)
                {
                    return contains_var(if_.cond, var)
                        or contains_var(if_.true_branch, var)
                        or if_.false_branch and contains_var(*if_.false_branch, var);
                },
                [&] (typename stmt_t<P>::return_t const& ret)
                {
                    return ret.expr and contains_var(*ret.expr, var);
                });
        });
}

template <Properties P>
bool contains_var(expr_t<P> const& x, typename expr_t<P>::var_t const& var)
{
    return match(
        x.value,
        [&] (typename expr_t<P>::typename_t const&) { return false; },
        [&] (typename expr_t<P>::bool_t const&) { return false; },
        [&] (typename expr_t<P>::unit_t const&) { return false; },
        [&] (typename expr_t<P>::i8_t const&) { return false; },
        [&] (typename expr_t<P>::i16_t const&) { return false; },
        [&] (typename expr_t<P>::i32_t const&) { return false; },
        [&] (typename expr_t<P>::i64_t const&) { return false; },
        [&] (typename expr_t<P>::u8_t const&) { return false; },
        [&] (typename expr_t<P>::u16_t const&) { return false; },
        [&] (typename expr_t<P>::u32_t const&) { return false; },
        [&] (typename expr_t<P>::u64_t const&) { return false; },
        [&] (typename expr_t<P>::boolean_constant_t const&) { return false; },
        [&] (typename expr_t<P>::numeric_constant_t const&) { return false; },
        [&] (typename expr_t<P>::arith_expr_t const& x)
        {
            return match(
                x.value,
                [&] (typename expr_t<P>::arith_expr_t::plus_t const& x)
                {
                    return contains_var(x.lhs.get(), var) or contains_var(x.rhs.get(), var);
                });
        },
        [&] (typename expr_t<P>::var_t const& x)
        {
            return x == var;
        },
        [&] (typename expr_t<P>::app_t const& x)
        {
            return contains_var<P>(x, var);
        },
        [&] (typename expr_t<P>::abs_t const& x)
        {
            return contains_var(x.args.begin(), x.args.end(), x.ret_type.get(), var) or contains_var(x.body, var);
        },
        [&] (typename expr_t<P>::pi_t const& x)
        {
            return contains_var(x.args.begin(), x.args.end(), x.ret_type.get(), var);
        });
}

template <Properties P>
bool contains_var(typename expr_t<P>::app_t const& x, typename expr_t<P>::var_t const& var)
{
    return std::ranges::any_of(x.args, [&] (expr_t<P> const& arg) { return contains_var(arg, var); }) or
        contains_var(x.func.get(), var);
}

template <Properties P>
bool contains_var(
    typename expr_t<P>::abs_t::arg_const_iterator begin,
    typename expr_t<P>::abs_t::arg_const_iterator end,
    expr_t<P> const& ret_type,
    typename expr_t<P>::var_t const& var)
{
    return std::any_of(begin, end, [&] (func_arg_t<P> const& arg) { return contains_var(arg, var); })
        or contains_var(ret_type, var);
}

} // namespace dep0::ast


