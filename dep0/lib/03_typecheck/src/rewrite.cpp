/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/rewrite.hpp"

#include "dep0/ast/alpha_equivalence.hpp"
#include "dep0/ast/occurs_in.hpp"

#include "dep0/fmap.hpp"
#include "dep0/match.hpp"

#include <boost/hana.hpp>

#include <algorithm>
#include <cassert>
#include <iterator>
#include <ranges>

namespace dep0::typecheck {

namespace impl {

template <typename T>
T choose(std::optional<T>&& new_value, T const& old_value)
{
    return new_value ? T(std::move(*new_value)) : T(old_value);
}

template <typename T>
std::vector<T> choose(std::vector<std::optional<T>>&& new_values, std::vector<T> const& old_values)
{
    assert(new_values.size() == old_values.size());
    std::vector<T> result;
    result.reserve(new_values.size());
    for (auto const i: std::views::iota(0ul, new_values.size()))
        if (auto& new_v = new_values[i])
            result.emplace_back(std::move(*new_v));
        else
            result.emplace_back(old_values[i]);
    return result;
}

template <typename T>
bool has_any_value(std::vector<std::optional<T>> const& xs)
{
    return std::ranges::any_of(xs, [] (std::optional<T> const& x) { return x.has_value(); });
}

std::optional<body_t> rewrite(expr_t const& from, expr_t const& to, body_t const&);
std::optional<stmt_t> rewrite(expr_t const& from, expr_t const& to, stmt_t const&);
std::optional<expr_t> rewrite(expr_t const& from, expr_t const& to, expr_t const&);

std::optional<expr_t::app_t> rewrite(expr_t const& from, expr_t const& to, expr_t::app_t const&);

void rewrite(
    expr_t const& from,
    expr_t const& to,
    ast::is_mutable_t const is_mutable,
    std::vector<func_arg_t> const& args,
    expr_t const& ret_type,
    body_t const* body,
    legal_expr_t const& old_properties,
    std::optional<expr_t>& result);

std::optional<body_t> rewrite(expr_t const& from, expr_t const& to, body_t const& old)
{
    std::optional<body_t> result;
    auto new_stmts = fmap(old.stmts, [&] (stmt_t const& x) { return rewrite(from, to, x); });
    if (has_any_value(new_stmts))
        result.emplace(old.properties, choose(std::move(new_stmts), old.stmts));
    return result;
}

std::optional<stmt_t> rewrite(expr_t const& from, expr_t const& to, stmt_t const& old)
{
    std::optional<stmt_t> result;
    match(
        old.value,
        [&] (expr_t::app_t const& app)
        {
            if (auto new_app = rewrite(from, to, app))
                result.emplace(old.properties, std::move(*new_app));
        },
        [&] (stmt_t::if_else_t const& if_else)
        {
            auto new_cond = rewrite(from, to, if_else.cond);
            auto new_true_branch = rewrite(from, to, if_else.true_branch);
            if (if_else.false_branch)
            {
                auto new_false_branch = rewrite(from, to, *if_else.false_branch);
                if (new_cond or new_true_branch or new_false_branch)
                    result.emplace(
                        old.properties,
                        stmt_t::if_else_t{
                            choose(std::move(new_cond), if_else.cond),
                            choose(std::move(new_true_branch), if_else.true_branch),
                            choose(std::move(new_false_branch), *if_else.false_branch)
                        });
            }
            else if (new_cond or new_true_branch)
                result.emplace(
                    old.properties,
                    stmt_t::if_else_t{
                        choose(std::move(new_cond), if_else.cond),
                        choose(std::move(new_true_branch), if_else.true_branch),
                        std::nullopt
                    });
        },
        [&] (stmt_t::return_t const& ret)
        {
            if (ret.expr)
                if (auto new_expr = rewrite(from, to, *ret.expr))
                    result.emplace(old.properties, stmt_t::return_t{std::move(*new_expr)});
        },
        [&] (stmt_t::impossible_t const& x)
        {
            if (x.reason)
                if (auto new_reason = rewrite(from, to, *x.reason))
                    result.emplace(old.properties, stmt_t::impossible_t{std::move(*new_reason)});
        });
    return result;
}

std::optional<expr_t> rewrite(expr_t const& from, expr_t const& to, expr_t const& old)
{
    std::optional<expr_t> result;
    if (ast::is_alpha_equivalent(old, from))
        result.emplace(to);
    else
        match(
            old.value,
            // leaf nodes (eg typename_t, var_t, etc) are empty because alpha-equivalence returned false,
            // meaning that `from != old` and therefore there is nothing to rewrite
            [] (expr_t::typename_t) { },
            [] (expr_t::true_t) { },
            [] (expr_t::auto_t) { },
            [] (expr_t::bool_t) { },
            [] (expr_t::cstr_t) { },
            [] (expr_t::unit_t) { },
            [] (expr_t::i8_t) { },
            [] (expr_t::i16_t) { },
            [] (expr_t::i32_t) { },
            [] (expr_t::i64_t) { },
            [] (expr_t::u8_t) { },
            [] (expr_t::u16_t) { },
            [] (expr_t::u32_t) { },
            [] (expr_t::u64_t) { },
            [] (expr_t::boolean_constant_t){},
            [] (expr_t::numeric_constant_t){},
            [] (expr_t::string_literal_t){},
            [&] (expr_t::boolean_expr_t const& x)
            {
                match(
                    x.value,
                    [&] (expr_t::boolean_expr_t::not_t const& x)
                    {
                        if (auto new_expr = rewrite(from, to, x.expr.get()))
                            result.emplace(
                                old.properties,
                                expr_t::boolean_expr_t{
                                    expr_t::boolean_expr_t::not_t{
                                        std::move(*new_expr)}});
                    },
                    [&] <typename T> (T const& x)
                    {
                        auto new_lhs = rewrite(from, to, x.lhs.get());
                        auto new_rhs = rewrite(from, to, x.rhs.get());
                        if (new_lhs or new_rhs)
                            result.emplace(
                                old.properties,
                                expr_t::boolean_expr_t{T{
                                    choose(std::move(new_lhs), x.lhs.get()),
                                    choose(std::move(new_rhs), x.rhs.get())}});
                    });
            },
            [&] (expr_t::relation_expr_t const& x)
            {
                match(
                    x.value,
                    [&] <typename T> (T const& x)
                    {
                        auto new_lhs = rewrite(from, to, x.lhs.get());
                        auto new_rhs = rewrite(from, to, x.rhs.get());
                        if (new_lhs or new_rhs)
                            result.emplace(
                                old.properties,
                                expr_t::relation_expr_t{T{
                                    choose(std::move(new_lhs), x.lhs.get()),
                                    choose(std::move(new_rhs), x.rhs.get())}});
                    });
            },
            [&] (expr_t::arith_expr_t const& x)
            {
                match(
                    x.value,
                    [&] <typename T> (T const& x)
                    {
                        auto new_lhs = rewrite(from, to, x.lhs.get());
                        auto new_rhs = rewrite(from, to, x.rhs.get());
                        if (new_lhs or new_rhs)
                            result.emplace(
                                old.properties,
                                expr_t::arith_expr_t{T{
                                    choose(std::move(new_lhs), x.lhs.get()),
                                    choose(std::move(new_rhs), x.rhs.get())}});
                    });
            },
            [] (expr_t::var_t const&) { },
            [] (expr_t::global_t const&) { },
            [&] (expr_t::app_t const& x)
            {
                if (auto new_app = rewrite(from, to, x))
                    result.emplace(old.properties, std::move(*new_app));
            },
            [&] (expr_t::abs_t const& x)
            {
                rewrite(from, to, x.is_mutable, x.args, x.ret_type.get(), &x.body, old.properties, result);
            },
            [&] (expr_t::pi_t const& x)
            {
                rewrite(from, to, x.is_mutable, x.args, x.ret_type.get(), nullptr, old.properties, result);
            },
            [&] (expr_t::array_t const&)
            {
            },
            [&] (expr_t::init_list_t const& x)
            {
                for (auto& v: x.values)
                    rewrite(from, to, v);
            },
            [&] (expr_t::subscript_t const& x)
            {
                rewrite(from, to, x.array.get());
                rewrite(from, to, x.index.get());
            },
            [&] (expr_t::because_t const& x)
            {
                rewrite(from, to, x.value.get());
                rewrite(from, to, x.reason.get());
            });
    return result;
}

std::optional<expr_t::app_t> rewrite(expr_t const& from, expr_t const& to, expr_t::app_t const& old)
{
    std::optional<expr_t::app_t> result;
    auto new_func = rewrite(from, to, old.func.get());
    auto new_args = fmap(old.args, [&] (expr_t const& x) { return rewrite(from, to, x); });
    if (new_func or has_any_value(new_args))
        result.emplace(
            choose(std::move(new_func), old.func.get()),
            choose(std::move(new_args), old.args));
    return result;
}

void rewrite(
    expr_t const& from,
    expr_t const& to,
    ast::is_mutable_t const is_mutable,
    std::vector<func_arg_t> const& old_args,
    expr_t const& old_ret_type,
    body_t const* old_body,
    legal_expr_t const& old_properties,
    std::optional<expr_t>& result)
{
    // Rewriting must stop if a binding variable occurs free in either `from` or `to`, because:
    // 1. in `from`, for consistency with alpha-equivalence, i.e. if it was named differently we would not rewrite;
    // 2. in `to`, for the same reason as term substition, i.e. it would change the meaning of the overall type.
    bool stop = false;
    auto new_args =
        fmap(
            old_args,
            [&] (func_arg_t const& old_arg)
            {
                std::optional<func_arg_t> new_arg;
                if (not stop)
                {
                    auto new_type = rewrite(from, to, old_arg.type);
                    if (old_arg.var)
                        stop |=
                            ast::occurs_in(*old_arg.var, from, ast::occurrence_style::free) or
                            ast::occurs_in(*old_arg.var, to, ast::occurrence_style::free);
                    if (new_type)
                        new_arg.emplace(old_arg.properties, old_arg.qty, std::move(*new_type), old_arg.var);
                }
                return new_arg;
            });
    auto new_ret_type = stop ? std::nullopt : rewrite(from, to, old_ret_type);
    if (old_body)
    {
        auto new_body = stop ? std::nullopt : rewrite(from, to, *old_body);
        if (new_ret_type or new_body or has_any_value(new_args))
            result.emplace(
                old_properties,
                expr_t::abs_t{
                    is_mutable,
                    choose(std::move(new_args), old_args),
                    choose(std::move(new_ret_type), old_ret_type),
                    choose(std::move(new_body), *old_body)});
    }
    else if (new_ret_type or has_any_value(new_args))
        result.emplace(
            old_properties,
            expr_t::pi_t{
                is_mutable,
                choose(std::move(new_args), old_args),
                choose(std::move(new_ret_type), old_ret_type)});
}

} // namespace impl

std::optional<sort_t> rewrite(expr_t const& from, expr_t const& to, sort_t const& x)
{
    std::optional<sort_t> result;
    if (auto const expr = std::get_if<expr_t>(&x))
        if (auto new_expr = impl::rewrite(from, to, *expr))
            result.emplace(std::move(*new_expr));
    return result;
}

} // namespace dep0::typecheck
