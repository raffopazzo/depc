/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Implementation details of @ref occurs_in.hpp
 */
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
                },
                [&] (stmt_t<P>::impossible_t const& x)
                {
                    return x.reason and occurs_in(var, *x.reason, style);
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
        [] (expr_t<P>::true_t const&) { return false; },
        [] (expr_t<P>::auto_t const&) { return false; },
        [] (expr_t<P>::bool_t const&) { return false; },
        [] (expr_t<P>::cstr_t const&) { return false; },
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
        [] (expr_t<P>::string_literal_t const&) { return false; },
        [&] (expr_t<P>::boolean_expr_t const& x)
        {
            return match(
                x.value,
                [&] (expr_t<P>::boolean_expr_t::not_t const& x)
                {
                    return occurs_in(var, x.expr.get(), style);
                },
                [&] (auto const& x)
                {
                    return occurs_in(var, x.lhs.get(), style) or occurs_in(var, x.rhs.get(), style);
                });
        },
        [&] (expr_t<P>::relation_expr_t const& x)
        {
            return match(
                x.value,
                [&] (auto const& x)
                {
                    return occurs_in(var, x.lhs.get(), style) or occurs_in(var, x.rhs.get(), style);
                });
        },
        [&] (expr_t<P>::arith_expr_t const& x)
        {
            return match(
                x.value,
                [&] (auto const& x)
                {
                    return occurs_in(var, x.lhs.get(), style) or occurs_in(var, x.rhs.get(), style);
                });
        },
        [&] (expr_t<P>::var_t const& x)
        {
            return x == var;
        },
        [] (expr_t<P>::global_t const&)
        {
            return false;
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
        [&] (expr_t<P>::sigma_t const& x)
        {
            return occurs_in<P>(var, x.args.begin(), x.args.end(), style);
        },
        [] (expr_t<P>::ref_t const&) { return false; },
        [] (expr_t<P>::scope_t const&) { return false; },
        [&] (expr_t<P>::addressof_t const& x) { return occurs_in(var, x.expr.get(), style); },
        [&] (expr_t<P>::deref_t const& x) { return occurs_in(var, x.expr.get(), style); },
        [&] (expr_t<P>::scopeof_t const& x) { return occurs_in(var, x.expr.get(), style); },
        [] (expr_t<P>::array_t const&)
        {
            return false;
        },
        [&] (expr_t<P>::init_list_t const& x)
        {
            return std::ranges::any_of(x.values, [&] (expr_t<P> const& v) { return occurs_in(var, v, style); });
        },
        [&] (expr_t<P>::member_t const& x)
        {
            return occurs_in(var, x.object.get(), style);
        },
        [&] (expr_t<P>::subscript_t const& x)
        {
            return occurs_in(var, x.object.get(), style) or occurs_in(var, x.index.get(), style);
        },
        [&] (expr_t<P>::because_t const& x)
        {
            return occurs_in(var, x.value.get(), style) or occurs_in(var, x.reason.get(), style);
        });
}

template <Properties P>
bool occurs_in(
    typename expr_t<P>::var_t const& var,
    typename std::vector<func_arg_t<P>>::const_iterator begin,
    typename std::vector<func_arg_t<P>>::const_iterator end,
    occurrence_style const style)
{
    for (auto const& arg: std::ranges::subrange(begin, end))
    {
        if (occurs_in(var, arg.type, style))
            return true;
        if (arg.var == var)
            // If we are looking for occurrences anywhere, return true because this is a valid one.
            // If we are looking for free occurrences, return false because any later occurrence is now bound.
            return style == occurrence_style::anywhere;
    }
    return false;
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
    // NB cannot remove duplication by invoking the above overload,
    // because if we are looking for free occurrences,
    // we have to fail immediately when a shadowing argument is introduced,
    // otherwise we might erroneously determine that a variable
    // occurs free in the return type when it is actually bound.
    for (auto const& arg: std::ranges::subrange(begin, end))
    {
        if (occurs_in(var, arg.type, style))
            return true;
        if (arg.var == var)
            // If we are looking for occurrences anywhere, return true because this is a valid one.
            // If we are looking for free occurrences, return false because any later occurrence is now bound.
            return style == occurrence_style::anywhere;
    }
    return occurs_in(var, ret_type, style) or (body and impl::occurs_in(var, *body, style));
}

} // namespace dep0::ast

