#pragma once

#include "dep0/ast/replace.hpp"

#include "dep0/match.hpp"

#include <ranges>

namespace dep0::ast {

template <Properties P>
void replace(typename type_t<P>::var_t const& from, typename type_t<P>::var_t const& to, type_t<P>& type)
{
    match(
        type.value,
        [] (typename type_t<P>::bool_t const&) { },
        [] (typename type_t<P>::unit_t const&) { },
        [] (typename type_t<P>::i8_t const&) { },
        [] (typename type_t<P>::i16_t const&) { },
        [] (typename type_t<P>::i32_t const&) { },
        [] (typename type_t<P>::i64_t const&) { },
        [] (typename type_t<P>::u8_t const&) { },
        [] (typename type_t<P>::u16_t const&) { },
        [] (typename type_t<P>::u32_t const&) { },
        [] (typename type_t<P>::u64_t const&) { },
        [&] (typename type_t<P>::var_t& v)
        {
            if (v == from)
                v = to;
        },
        [&] (typename type_t<P>::arr_t& arr)
        {
            replace(from, to, arr.args.begin(), arr.args.end(), arr.ret_type.get());
        });
}

template <Properties P>
void replace(
    typename type_t<P>::var_t const& from,
    typename type_t<P>::var_t const& to,
    typename type_t<P>::arr_t::arg_iterator const begin,
    typename type_t<P>::arr_t::arg_iterator const end,
    type_t<P>& ret_type)
{
    for (auto& arg: std::ranges::subrange(begin, end))
        match(
            arg.value,
            [&] (typename func_arg_t<P>::type_arg_t& arg)
            {
                // Note: in theory it would suffice to replace only the free occurrences of `from`
                // and we could stop if `from` introduces a new binding variable;
                // but replacing everything is easier and it shouldn't harm.
                if (arg.var == from)
                    arg.var = to;
            },
            [&] (typename func_arg_t<P>::term_arg_t& arg)
            {
                replace(from, to, arg.type);
            });
    replace(from, to, ret_type);
}

template <Properties P>
void replace(typename expr_t<P>::var_t const& from, typename expr_t<P>::var_t const& to, type_t<P>& x)
{
    match(
        x.value,
        [] (typename type_t<P>::bool_t const&) { },
        [] (typename type_t<P>::unit_t const&) { },
        [] (typename type_t<P>::i8_t const&) { },
        [] (typename type_t<P>::i16_t const&) { },
        [] (typename type_t<P>::i32_t const&) { },
        [] (typename type_t<P>::i64_t const&) { },
        [] (typename type_t<P>::u8_t const&) { },
        [] (typename type_t<P>::u16_t const&) { },
        [] (typename type_t<P>::u32_t const&) { },
        [] (typename type_t<P>::u64_t const&) { },
        [] (typename type_t<P>::var_t const&) { },
        [&] (typename type_t<P>::arr_t& arr)
        {
            for (auto& arg: arr.args)
                match(
                    arg.value,
                    [](typename func_arg_t<P>::type_arg_t const&) {},
                    [&](typename func_arg_t<P>::term_arg_t& arg)
                    {
                        replace(from, to, arg.type);
                        if (arg.var == from)
                            arg.var = to;
                    });
            replace(from, to, arr.ret_type.get());
        });
}

template <Properties P>
void replace(typename expr_t<P>::var_t const& from, typename expr_t<P>::var_t const& to, body_t<P>& x)
{
    for (auto& s: x.stmts)
        match(
            s.value,
            [&] (typename expr_t<P>::app_t& app)
            {
                return replace<P>(from, to, app);
            },
            [&] (typename stmt_t<P>::if_else_t& if_)
            {
                replace(from, to, if_.cond);
                replace(from, to, if_.true_branch);
                if (if_.false_branch)
                    replace(from, to, *if_.false_branch);
            },
            [&] (typename stmt_t<P>::return_t& ret)
            {
                if (ret.expr)
                    replace(from, to, *ret.expr);
            });
}

template <Properties P>
void replace(typename expr_t<P>::var_t const& from, typename expr_t<P>::var_t const& to, expr_t<P>& x)
{
    match(
        x.value,
        [&] (typename expr_t<P>::arith_expr_t& x)
        {
            return match(
                x.value,
                [&] (typename expr_t<P>::arith_expr_t::plus_t& x)
                {
                    replace(from, to, x.lhs.get());
                    replace(from, to, x.rhs.get());
                });
        },
        [&] (typename expr_t<P>::boolean_constant_t const&) { },
        [&] (typename expr_t<P>::numeric_constant_t const&) { },
        [&] (typename expr_t<P>::var_t& v)
        {
            if (v == from)
                v = to;
        },
        [&] (typename expr_t<P>::app_t& app)
        {
            return replace<P>(from, to, app);
        },
        [&] (typename expr_t<P>::abs_t& x)
        {
            // Note: in theory it would suffice to replace only the free occurrences of `from`
            // and we could stop if `from` introduces a new binding variable;
            // but replacing everything is easier and it shouldn't harm.
            for (auto& arg: x.args)
                match(
                    arg.value,
                    [] (typename func_arg_t<P>::type_arg_t const&) { },
                    [&] (typename func_arg_t<P>::term_arg_t& arg)
                    {
                        if (arg.var == from)
                            arg.var = to;
                        replace(from, to, arg.type);
                    });
            replace(from, to, x.body);
        },
        [&] (type_t<P>& x)
        {
            replace(from, to, x);
        });
}

template <Properties P>
void replace(typename expr_t<P>::var_t const& from, typename expr_t<P>::var_t const& to, typename expr_t<P>::app_t& x)
{
    replace(from, to, x.func.get());
    for (auto& arg: x.args)
        replace(from, to, arg);
}

} // namespace dep0::ast
