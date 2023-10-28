#pragma once

#include "dep0/ast/contains_var.hpp"

#include "dep0/match.hpp"

#include <algorithm>

namespace dep0::ast {

template <Properties P>
bool contains_var(type_t<P> const& type, typename type_t<P>::var_t const& var)
{
    return match(
        type.value,
        [] (typename type_t<P>::bool_t const&) { return false; },
        [] (typename type_t<P>::unit_t const&) { return false; },
        [] (typename type_t<P>::i8_t const&) { return false; },
        [] (typename type_t<P>::i16_t const&) { return false; },
        [] (typename type_t<P>::i32_t const&) { return false; },
        [] (typename type_t<P>::i64_t const&) { return false; },
        [] (typename type_t<P>::u8_t const&) { return false; },
        [] (typename type_t<P>::u16_t const&) { return false; },
        [] (typename type_t<P>::u32_t const&) { return false; },
        [] (typename type_t<P>::u64_t const&) { return false; },
        [&] (typename type_t<P>::var_t const& v)
        {
            return v == var;
        },
        [&] (typename type_t<P>::arr_t const& arr)
        {
            return contains_var(arr.ret_type.get(), var) or
                std::ranges::any_of(arr.args, [&] (func_arg_t<P> const& arg) { return contains_var(arg, var); });
        });
}

template <Properties P>
bool contains_var(type_t<P> const& type, typename expr_t<P>::var_t const& var)
{
    return match(
        type.value,
        [] (typename type_t<P>::bool_t const&) { return false; },
        [] (typename type_t<P>::unit_t const&) { return false; },
        [] (typename type_t<P>::i8_t const&) { return false; },
        [] (typename type_t<P>::i16_t const&) { return false; },
        [] (typename type_t<P>::i32_t const&) { return false; },
        [] (typename type_t<P>::i64_t const&) { return false; },
        [] (typename type_t<P>::u8_t const&) { return false; },
        [] (typename type_t<P>::u16_t const&) { return false; },
        [] (typename type_t<P>::u32_t const&) { return false; },
        [] (typename type_t<P>::u64_t const&) { return false; },
        [] (typename type_t<P>::var_t const&) { return false; },
        [&] (typename type_t<P>::arr_t const& arr)
        {
            return std::ranges::any_of(arr.args, [&] (func_arg_t<P> const& arg) { return contains_var(arg, var); }) or
                contains_var(arr.ret_type.get(), var);
        });
}

template <Properties P>
bool contains_var(func_arg_t<P> const& x, typename type_t<P>::var_t const& var)
{
    return match(
        x.value,
        [&] (typename func_arg_t<P>::type_arg_t const& arg) { return arg.var == var; },
        [&] (typename func_arg_t<P>::term_arg_t const& arg) { return contains_var(arg.type, var); });
}

template <Properties P>
bool contains_var(func_arg_t<P> const& x, typename expr_t<P>::var_t const& var)
{
    return match(
        x.value,
        [] (typename func_arg_t<P>::type_arg_t const&) { return false; },
        [&] (typename func_arg_t<P>::term_arg_t const& arg)
        {
            return contains_var(arg.type, var) or arg.var == var;
        });
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
        [&] (typename expr_t<P>::arith_expr_t const& x)
        {
            return match(
                x.value,
                [&] (typename expr_t<P>::arith_expr_t::plus_t const& x)
                {
                    return contains_var(x.lhs.get(), var) or contains_var(x.rhs.get(), var);
                });
        },
        [&] (typename expr_t<P>::boolean_constant_t const&) { return false; },
        [&] (typename expr_t<P>::numeric_constant_t const&) { return false; },
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
            return std::ranges::any_of(x.args, [&] (func_arg_t<P> const& arg) { return contains_var(arg, var); })
                or contains_var(x.body, var);
        },
        [&] (type_t<P> const& x)
        {
            return contains_var(x, var);
        });
}

template <Properties P>
bool contains_var(typename expr_t<P>::app_t const& x, typename expr_t<P>::var_t const& var)
{
    return contains_var(x.func.get(), var)
        or std::ranges::any_of(x.args, [&] (expr_t<P> const& arg) { return contains_var(arg, var); });
}

} // namespace dep0::ast


