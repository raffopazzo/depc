/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/max_scope.hpp"

#include "dep0/error.hpp"
#include "dep0/match.hpp"

#include "dep0/ast/pretty_print.hpp"

#include <algorithm>
#include <numeric>

namespace dep0::typecheck {

namespace impl {

static expected<std::size_t> max_scope_combine(expected<std::size_t>, expected<std::size_t>);
static expected<std::size_t> max_scope_combine(expected<std::size_t>, expected<std::size_t>, expected<std::size_t>);
static expected<std::size_t> max_scope_args(ctx_t const&, std::vector<func_arg_t> const&);
static expected<std::size_t> max_scope_app(ctx_t const&, expr_t::app_t const&);

static expected<std::size_t> max_scope_body(ctx_t const&, body_t const&);
static expected<std::size_t> max_scope_stmt(ctx_t const&, stmt_t const&);
static expected<std::size_t> max_scope_expr(ctx_t const&, expr_t const&);

template <typename T, typename F>
expected<std::size_t> max_scope_vector(ctx_t const& ctx, std::vector<T> const& xs, F&& proj)
{
    return std::accumulate(
        xs.begin(), xs.end(),
        expected<std::size_t>{0ul},
        [&](expected<std::size_t> acc, auto const& item) -> expected<std::size_t>
        {
            return max_scope_combine(std::move(acc), max_scope_expr(ctx, proj(item)));
        });
}

expected<std::size_t> max_scope_combine(expected<std::size_t> lhs, expected<std::size_t> rhs)
{
    if (not lhs)
        return std::move(lhs.error());
    if (not rhs)
        return std::move(rhs.error());
    return std::max(*lhs, *rhs);
}

expected<std::size_t> max_scope_combine(expected<std::size_t> a, expected<std::size_t> b, expected<std::size_t> c)
{
    return max_scope_combine(max_scope_combine(std::move(a), std::move(b)), std::move(c));
}


expected<std::size_t> max_scope_args(ctx_t const& ctx, std::vector<func_arg_t> const& args)
{
    return max_scope_vector(ctx, args, [](func_arg_t const& arg) -> expr_t const& { return arg.type; });
}

expected<std::size_t> max_scope_app(ctx_t const& ctx, expr_t::app_t const&)
{
    // The result of a function application is always a temporary value in the current scope.
    if (ctx.scope())
        return *ctx.scope();
    else
        return error_t("result of function application cannot be bound to an unscoped context");
}

expected<std::size_t> max_scope_body(ctx_t const& ctx, body_t const& body)
{
    return std::accumulate(
        body.stmts.begin(), body.stmts.end(),
        expected<std::size_t>{0ul},
        [&](expected<std::size_t> acc, stmt_t const& stmt) -> expected<std::size_t>
        {
            return max_scope_combine(std::move(acc), max_scope_stmt(ctx, stmt));
        });
}

expected<std::size_t> max_scope_stmt(ctx_t const& ctx, stmt_t const& stmt)
{
    return match(
        stmt.value,
        [&] (expr_t::app_t const& app) -> expected<std::size_t>
        {
            return max_scope_app(ctx, app);
        },
        [&] (stmt_t::if_else_t const& if_stmt) -> expected<std::size_t>
        {
            auto cond_result = max_scope_expr(ctx, if_stmt.cond);
            auto true_result = max_scope_body(ctx, if_stmt.true_branch);
            return if_stmt.false_branch
                ? max_scope_combine(
                    std::move(cond_result),
                    std::move(true_result),
                    max_scope_body(ctx, *if_stmt.false_branch))
                : max_scope_combine(
                    std::move(cond_result),
                    std::move(true_result));
        },
        [&] (stmt_t::return_t const& ret_stmt) -> expected<std::size_t>
        {
            if (ret_stmt.expr)
                return max_scope_expr(ctx, *ret_stmt.expr);
            else
                return std::size_t{0ul};
        },
        [&] (stmt_t::impossible_t const& imp_stmt) -> expected<std::size_t>
        {
            if (imp_stmt.reason)
                return max_scope_expr(ctx, *imp_stmt.reason);
            else
                return std::size_t{0ul};
        });
}

expected<std::size_t> max_scope_expr(ctx_t const& ctx, expr_t const& expr)
{
    return match(
        expr.value,
        [] (expr_t::typename_t const&) -> expected<std::size_t> { return std::size_t{0ul}; },
        [] (expr_t::true_t const&) -> expected<std::size_t> { return std::size_t{0ul}; },
        [] (expr_t::auto_t const&) -> expected<std::size_t> { return std::size_t{0ul}; },
        [] (expr_t::bool_t const&) -> expected<std::size_t> { return std::size_t{0ul}; },
        [] (expr_t::cstr_t const&) -> expected<std::size_t> { return std::size_t{0ul}; },
        [] (expr_t::unit_t const&) -> expected<std::size_t> { return std::size_t{0ul}; },
        [] (expr_t::i8_t const&) -> expected<std::size_t> { return std::size_t{0ul}; },
        [] (expr_t::i16_t const&) -> expected<std::size_t> { return std::size_t{0ul}; },
        [] (expr_t::i32_t const&) -> expected<std::size_t> { return std::size_t{0ul}; },
        [] (expr_t::i64_t const&) -> expected<std::size_t> { return std::size_t{0ul}; },
        [] (expr_t::u8_t const&) -> expected<std::size_t> { return std::size_t{0ul}; },
        [] (expr_t::u16_t const&) -> expected<std::size_t> { return std::size_t{0ul}; },
        [] (expr_t::u32_t const&) -> expected<std::size_t> { return std::size_t{0ul}; },
        [] (expr_t::u64_t const&) -> expected<std::size_t> { return std::size_t{0ul}; },
        [] (expr_t::boolean_constant_t const&) -> expected<std::size_t> { return std::size_t{0ul}; },
        [] (expr_t::numeric_constant_t const&) -> expected<std::size_t> { return std::size_t{0ul}; },
        [] (expr_t::string_literal_t const&) -> expected<std::size_t> { return std::size_t{0ul}; },
        [&] (expr_t::boolean_expr_t const& bool_expr) -> expected<std::size_t>
        {
            return match(
                bool_expr.value,
                [&] (expr_t::boolean_expr_t::not_t const& not_expr) -> expected<std::size_t>
                {
                    return max_scope_expr(ctx, not_expr.expr.get());
                },
                [&] (auto const& binary_expr) -> expected<std::size_t>
                {
                    return max_scope_combine(
                        max_scope_expr(ctx, binary_expr.lhs.get()),
                        max_scope_expr(ctx, binary_expr.rhs.get()));
                });
        },
        [&] (expr_t::relation_expr_t const& rel_expr) -> expected<std::size_t>
        {
            return match(
                rel_expr.value,
                [&] (auto const& binary_expr) -> expected<std::size_t>
                {
                    return max_scope_combine(
                        max_scope_expr(ctx, binary_expr.lhs.get()),
                        max_scope_expr(ctx, binary_expr.rhs.get()));
                });
        },
        [&] (expr_t::arith_expr_t const& arith_expr) -> expected<std::size_t>
        {
            return match(
                arith_expr.value,
                [&] (auto const& binary_expr) -> expected<std::size_t>
                {
                    return max_scope_combine(
                        max_scope_expr(ctx, binary_expr.lhs.get()),
                        max_scope_expr(ctx, binary_expr.rhs.get()));
                });
        },
        [&] (expr_t::var_t const& var) -> expected<std::size_t>
        {
            if (auto const val = ctx[var])
            {
                if (val->scope_id)
                    return *val->scope_id;
                else
                {
                    std::ostringstream err;
                    ast::pretty_print<properties_t>(err << '`', var) << "` not bound to a scope";
                    return error_t{err.str()};
                }
            }
            else
            {
                std::ostringstream err;
                ast::pretty_print<properties_t>(err << "unknown variable `", var) << '`';
                return error_t{err.str()};
            }
        },
        [] (expr_t::global_t const&) -> expected<std::size_t>
        {
            return std::size_t{0ul};
        },
        [&] (expr_t::app_t const& app) -> expected<std::size_t>
        {
            return max_scope_app(ctx, app);
        },
        [&] (expr_t::abs_t const& abs) -> expected<std::size_t>
        {
            return max_scope_combine(
                 max_scope_args(ctx, abs.args),
                 max_scope_expr(ctx, abs.ret_type.get()),
                 max_scope_body(ctx, abs.body));
        },
        [&] (expr_t::pi_t const& pi) -> expected<std::size_t>
        {
            return max_scope_combine(
                max_scope_args(ctx, pi.args),
                max_scope_expr(ctx, pi.ret_type.get()));
        },
        [&] (expr_t::sigma_t const& sigma) -> expected<std::size_t>
        {
            return max_scope_args(ctx, sigma.args);
        },
        [] (expr_t::ref_t const&) -> expected<std::size_t> { return std::size_t{0ul}; },
        [] (expr_t::scope_t const&) -> expected<std::size_t> { return std::size_t{0ul}; },
        [&] (expr_t::addressof_t const& addr) -> expected<std::size_t>
        {
            return max_scope_expr(ctx, addr.expr.get());
        },
        [&] (expr_t::deref_t const& deref) -> expected<std::size_t>
        {
            return max_scope_expr(ctx, deref.expr.get());
        },
        [&] (expr_t::scopeof_t const& scopeof) -> expected<std::size_t>
        {
            return max_scope_expr(ctx, scopeof.expr.get());
        },
        [] (expr_t::array_t const&) -> expected<std::size_t>
        {
            return std::size_t{0ul};
        },
        [&] (expr_t::init_list_t const& init_list) -> expected<std::size_t>
        {
            return max_scope_vector(ctx, init_list.values, [](expr_t const& value) -> expr_t const& { return value; });
        },
        [&] (expr_t::member_t const& member) -> expected<std::size_t>
        {
            return max_scope_expr(ctx, member.object.get());
        },
        [&] (expr_t::subscript_t const& subscript) -> expected<std::size_t>
        {
            return max_scope_combine(
                max_scope_expr(ctx, subscript.object.get()),
                max_scope_expr(ctx, subscript.index.get()));
        },
        [&] (expr_t::because_t const& because) -> expected<std::size_t>
        {
            return max_scope_combine(
                max_scope_expr(ctx, because.value.get()),
                max_scope_expr(ctx, because.reason.get()));
        });
}

} // namespace impl

expected<std::size_t> max_scope(ctx_t const& ctx, expr_t const& expr)
{
    return impl::max_scope_expr(ctx, expr);
}

} // namespace dep0::typecheck
