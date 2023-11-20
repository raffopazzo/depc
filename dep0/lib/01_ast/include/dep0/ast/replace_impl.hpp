#pragma once

#include "dep0/ast/replace.hpp"

#include "dep0/match.hpp"

#include <ranges>

namespace dep0::ast {

template <Properties P>
void replace(
    typename expr_t<P>::var_t const& from,
    typename expr_t<P>::var_t const& to,
    typename std::vector<func_arg_t<P>>::iterator const begin,
    typename std::vector<func_arg_t<P>>::iterator const end,
    expr_t<P>& ret_type)
{
    for (auto& arg: std::ranges::subrange(begin, end))
    {
        replace(from, to, arg.type);
        // Note: in theory it would suffice to replace only the free occurrences of `from`
        // and we could stop if `from` introduces a new binding variable;
        // but replacing everything is easier and it shouldn't harm.
        if (arg.var == from)
            arg.var = to;
    }
    replace(from, to, ret_type);
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
        [] (typename expr_t<P>::typename_t const&) {},
        [] (typename expr_t<P>::bool_t const&) {},
        [] (typename expr_t<P>::unit_t const&) {},
        [] (typename expr_t<P>::i8_t const&) {},
        [] (typename expr_t<P>::i16_t const&) {},
        [] (typename expr_t<P>::i32_t const&) {},
        [] (typename expr_t<P>::i64_t const&) {},
        [] (typename expr_t<P>::u8_t const&) {},
        [] (typename expr_t<P>::u16_t const&) {},
        [] (typename expr_t<P>::u32_t const&) {},
        [] (typename expr_t<P>::u64_t const&) {},
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
            replace(from, to, x.args.begin(), x.args.end(), x.ret_type.get());
            replace(from, to, x.body);
        },
        [&] (typename expr_t<P>::pi_t& pi)
        {
            replace(from, to, pi.args.begin(), pi.args.end(), pi.ret_type.get());
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
