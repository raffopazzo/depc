/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
/**
 * @file
 * @brief Implementation details for @ref dep0/ast/size.hpp.
 */
#pragma once

#include "dep0/ast/size.hpp"

#include "dep0/match.hpp"

#include <algorithm>
#include <numeric>

namespace dep0::ast {

namespace impl {

template <Properties P> std::size_t size(body_t<P> const&);
template <Properties P> std::size_t size(stmt_t<P> const&);
template <Properties P> std::size_t size(typename expr_t<P>::app_t const&);
template <Properties P> std::size_t size(
    typename std::vector<func_arg_t<P>>::const_iterator begin,
    typename std::vector<func_arg_t<P>>::const_iterator end);
template <Properties P> std::size_t size(
    typename std::vector<func_arg_t<P>>::const_iterator begin,
    typename std::vector<func_arg_t<P>>::const_iterator end,
    expr_t<P> const& ret_type,
    body_t<P> const* body);

template <Properties P>
std::size_t size(body_t<P> const& x)
{
    return std::accumulate(
        x.stmts.begin(), x.stmts.end(),
        0ul,
        [] (std::size_t const acc, stmt_t<P> const& s)
        {
            return std::max(acc, size(s) );
        });
}

template <Properties P>
std::size_t size(stmt_t<P> const& x)
{
    return match(
        x.value,
        [] (expr_t<P>::app_t const& x)
        {
            return size<P>(x);
        },
        [] (stmt_t<P>::if_else_t const& if_)
        {
            return 1ul + std::max(
                size(if_.cond),
                std::max(size(if_.true_branch), if_.false_branch ? size(*if_.false_branch) : 0ul));
        },
        [] (stmt_t<P>::return_t const& ret)
        {
            return ret.expr ? 1ul + size(*ret.expr) : 1ul;
        },
        [] (stmt_t<P>::impossible_t const& x)
        {
            return x.reason ? 1ul + size(*x.reason) : 1ul;
        });
}

template <Properties P>
std::size_t size(typename expr_t<P>::app_t const& x)
{
    return 1ul + std::accumulate(
        x.args.begin(), x.args.end(),
        size(x.func.get()),
        [] (std::size_t const acc, expr_t<P> const& arg)
        {
            return std::max(acc, size(arg));
        });
}

template <Properties P>
std::size_t size(
    typename std::vector<func_arg_t<P>>::const_iterator const begin,
    typename std::vector<func_arg_t<P>>::const_iterator const end)
{
    return std::accumulate(
        begin, end, 0ul,
        [] (std::size_t const acc, func_arg_t<P> const& arg)
        {
            return std::max(acc, size(arg.type));
        });
}

template <Properties P>
std::size_t size(
    typename std::vector<func_arg_t<P>>::const_iterator const begin,
    typename std::vector<func_arg_t<P>>::const_iterator const end,
    expr_t<P> const& ret_type,
    body_t<P> const* body)
{
    return std::accumulate(
        begin, end,
        std::max(size(ret_type), body ? size(*body) : 0ul),
        [] (std::size_t const acc, func_arg_t<P> const& arg)
        {
            return std::max(acc, size(arg.type));
        });
}

} // namespace impl

template <Properties P>
std::size_t size(expr_t<P> const& x)
{
    return match(
        x.value,
        [] (expr_t<P>::typename_t const&) { return 0ul; },
        [] (expr_t<P>::true_t const&) { return 0ul; },
        [] (expr_t<P>::auto_t const&) { return 0ul; },
        [] (expr_t<P>::bool_t const&) { return 0ul; },
        [] (expr_t<P>::cstr_t const&) { return 0ul; },
        [] (expr_t<P>::unit_t const&) { return 0ul; },
        [] (expr_t<P>::i8_t const&) { return 0ul; },
        [] (expr_t<P>::i16_t const&) { return 0ul; },
        [] (expr_t<P>::i32_t const&) { return 0ul; },
        [] (expr_t<P>::i64_t const&) { return 0ul; },
        [] (expr_t<P>::u8_t const&) { return 0ul; },
        [] (expr_t<P>::u16_t const&) { return 0ul; },
        [] (expr_t<P>::u32_t const&) { return 0ul; },
        [] (expr_t<P>::u64_t const&) { return 0ul; },
        [] (expr_t<P>::boolean_constant_t const&) { return 0ul; },
        [] (expr_t<P>::numeric_constant_t const&) { return 0ul; },
        [] (expr_t<P>::string_literal_t const&) { return 0ul; },
        [] (expr_t<P>::boolean_expr_t const& x)
        {
            return 1ul + match(
                x.value,
                [] (expr_t<P>::boolean_expr_t::not_t const& x)
                {
                    return size(x.expr.get());
                },
                [] (auto const& x)
                {
                    return std::max(size(x.lhs.get()), size(x.rhs.get()));
                });
        },
        [] (expr_t<P>::relation_expr_t const& x)
        {
            return 1ul + match(x.value, [] (auto const& x) { return std::max(size(x.lhs.get()), size(x.rhs.get())); });
        },
        [] (expr_t<P>::arith_expr_t const& x)
        {
            return 1ul + match(x.value, [] (auto const& x) { return std::max(size(x.lhs.get()), size(x.rhs.get())); });
        },
        [] (expr_t<P>::var_t const&) { return 0ul; },
        [] (expr_t<P>::global_t const&) { return 0ul; },
        [] (expr_t<P>::app_t const& x) { return impl::size<P>(x); },
        [] (expr_t<P>::abs_t const& x)
        {
            return 1ul + impl::size<P>(x.args.begin(), x.args.end(), x.ret_type.get(), &x.body);
        },
        [] (expr_t<P>::pi_t const& x)
        {
            return 1ul + impl::size<P>(x.args.begin(), x.args.end(), x.ret_type.get(), nullptr);
        },
        [] (expr_t<P>::sigma_t const& x)
        {
            return 1ul + impl::size<P>(x.args.begin(), x.args.end());
        },
        [] (expr_t<P>::array_t const&)
        {
            return 0ul;
        },
        [] (expr_t<P>::init_list_t const& x)
        {
            return 1ul + std::accumulate(
                x.values.begin(), x.values.end(),
                0ul,
                [] (std::size_t const acc, expr_t<P> const& v)
                {
                    return std::max(acc, size(v));
                });
        },
        [] (expr_t<P>::subscript_t const& x)
        {
            return 1ul + std::max(size(x.array.get()), size(x.index.get()));
        },
        [] (expr_t<P>::because_t const& x)
        {
            return 1ul + std::max(size(x.value.get()), size(x.reason.get()));
        });
}

} // namespace dep0::ast
