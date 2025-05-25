/*
 * Copyright Raffaele Rossi 2023 - 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/substitute.hpp"

#include "dep0/ast/occurs_in.hpp"
#include "dep0/ast/rename.hpp"

#include "dep0/match.hpp"

#include <algorithm>

namespace dep0::typecheck {

namespace impl {

static void substitute(expr_t::var_t const&, expr_t const&, body_t&);
static void substitute(expr_t::var_t const&, expr_t const&, expr_t&);
static void substitute(expr_t::var_t const&, expr_t const&, expr_t::app_t&);

void substitute(expr_t::var_t const& var, expr_t const& expr, body_t& body)
{
    for (auto& stmt: body.stmts)
        match(
            stmt.value,
            [&] (expr_t::app_t& app)
            {
                substitute(var, expr, app);
            },
            [&] (stmt_t::if_else_t& if_)
            {
                substitute(var, expr, if_.cond);
                substitute(var, expr, if_.true_branch);
                if (if_.false_branch)
                    substitute(var, expr, *if_.false_branch);
            },
            [&] (stmt_t::return_t& ret)
            {
                if (ret.expr)
                    substitute(var, expr, *ret.expr);
            },
            [&] (stmt_t::impossible_t& x)
            {
                if (x.reason)
                    substitute(var, expr, *x.reason);
            });
}

void substitute(expr_t::var_t const& var, expr_t const& expr, expr_t& x)
{
    if (auto const type = std::get_if<expr_t>(&x.properties.sort.get()))
        substitute(var, expr, *type);
    match(
        x.value,
        [] (expr_t::typename_t const&) {},
        [] (expr_t::true_t const&) {},
        [] (expr_t::auto_t const&) {},
        [] (expr_t::ref_t const&) {},
        [] (expr_t::scope_t const&) {},
        [] (expr_t::addressof_t const&) {},
        [&] (expr_t::deref_t& x) { substitute(var, expr, x.ref.get()); },
        [] (expr_t::scopeof_t const&) {},
        [] (expr_t::bool_t const&) {},
        [] (expr_t::cstr_t const&) {},
        [] (expr_t::unit_t const&) {},
        [] (expr_t::i8_t const&) {},
        [] (expr_t::i16_t const&) {},
        [] (expr_t::i32_t const&) {},
        [] (expr_t::i64_t const&) {},
        [] (expr_t::u8_t const&) {},
        [] (expr_t::u16_t const&) {},
        [] (expr_t::u32_t const&) {},
        [] (expr_t::u64_t const&) {},
        [] (expr_t::boolean_constant_t const&) { },
        [] (expr_t::numeric_constant_t const&) { },
        [] (expr_t::string_literal_t const&) { },
        [&] (expr_t::boolean_expr_t& x)
        {
            match(
                x.value,
                [&] (expr_t::boolean_expr_t::not_t& x)
                {
                    substitute(var, expr, x.expr.get());
                },
                [&] (auto& x)
                {
                    substitute(var, expr, x.lhs.get());
                    substitute(var, expr, x.rhs.get());
                });
        },
        [&] (expr_t::relation_expr_t& x)
        {
            match(
                x.value,
                [&] (auto& x)
                {
                    substitute(var, expr, x.lhs.get());
                    substitute(var, expr, x.rhs.get());
                });
        },
        [&] (expr_t::arith_expr_t& x)
        {
            match(
                x.value,
                [&] (auto& x)
                {
                    substitute(var, expr, x.lhs.get());
                    substitute(var, expr, x.rhs.get());
                });
        },
        [&] (expr_t::var_t& v)
        {
            if (v == var)
                x = expr;
        },
        [] (expr_t::global_t const&)
        {
        },
        [&] (expr_t::app_t& x)
        {
            substitute(var, expr, x);
        },
        [&] (expr_t::abs_t& x)
        {
            substitute(var, expr, x.args.begin(), x.args.end(), x.ret_type.get(), &x.body);
        },
        [&] (expr_t::pi_t& x)
        {
            substitute(var, expr, x.args.begin(), x.args.end(), x.ret_type.get(), nullptr);
        },
        [&] (expr_t::sigma_t& x)
        {
            substitute(var, expr, x.args.begin(), x.args.end());
        },
        [] (expr_t::array_t const&)
        {
        },
        [&] (expr_t::init_list_t& x)
        {
            for (auto& v: x.values)
                substitute(var, expr, v);
        },
        [&] (expr_t::member_t& x)
        {
            substitute(var, expr, x.object.get());
        },
        [&] (expr_t::subscript_t& x)
        {
            substitute(var, expr, x.object.get());
            substitute(var, expr, x.index.get());
        },
        [&] (expr_t::because_t& x)
        {
            substitute(var, expr, x.value.get());
            substitute(var, expr, x.reason.get());
        });
}

void substitute(expr_t::var_t const& var, expr_t const& expr, expr_t::app_t& app)
{
    substitute(var, expr, app.func.get());
    for (auto& arg: app.args)
        substitute(var, expr, arg);
}

} // namespace impl

void substitute(
    expr_t::var_t const& var,
    expr_t const& y,
    std::vector<func_arg_t>::iterator it,
    std::vector<func_arg_t>::iterator const end)
{
    for (; it != end; ++it)
    {
        auto& arg = *it;
        impl::substitute(var, y, arg.type);
        if (arg.var == var)
        {
            // `arg.var` is now a new binding type-variable;
            // any later arguments refer to `arg.var` not to the initial `var`;
            // so substitution must stop, including for the return type
            return;
        }
        // We need to check if `arg.var` appears in `y` and, if so, rename it.
        // Technically it would suffice to check whether `arg.var` appears free in `y`,
        // but we prefer to check if it appears anywhere (i.e. also as binding variable)
        // because it renames possibly confusing types like `(typename t) -> (typename t) -> t` to
        // `(typename t:1) -> (typename t) -> t`, making it obvious to see which `t` is binding.
        // Also note that we are modifying the elements of the very vector we are iterating on,
        // but we are only modifying the values, not the vector; so iteration is safe.
        if (arg.var and ast::occurs_in(*arg.var, y, ast::occurrence_style::anywhere))
            arg.var = ast::rename<properties_t>(*arg.var, std::next(it), end);
    }
}

void substitute(
    expr_t::var_t const& var,
    expr_t const& y,
    std::vector<func_arg_t>::iterator it,
    std::vector<func_arg_t>::iterator const end,
    expr_t& ret_type,
    body_t* body)
{
    for (; it != end; ++it)
    {
        auto& arg = *it;
        impl::substitute(var, y, arg.type);
        if (arg.var == var)
        {
            // `arg.var` is now a new binding type-variable;
            // any later arguments refer to `arg.var` not to the initial `var`;
            // so substitution must stop, including for the return type
            return;
        }
        // We need to check if `arg.var` appears in `y` and, if so, rename it.
        // Technically it would suffice to check whether `arg.var` appears free in `y`,
        // but we prefer to check if it appears anywhere (i.e. also as binding variable)
        // because it renames possibly confusing types like `(typename t) -> (typename t) -> t` to
        // `(typename t:1) -> (typename t) -> t`, making it obvious to see which `t` is binding.
        // Also note that we are modifying the elements of the very vector we are iterating on,
        // but we are only modifying the values, not the vector; so iteration is safe.
        if (arg.var and ast::occurs_in(*arg.var, y, ast::occurrence_style::anywhere))
            arg.var = ast::rename(*arg.var, std::next(it), end, ret_type, body);
    }
    impl::substitute(var, y, ret_type);
    if (body)
        impl::substitute(var, y, *body);
}

void substitute(
    expr_t::var_t const& var,
    expr_t const& y,
    std::vector<type_def_t::struct_t::field_t>::iterator it,
    std::vector<type_def_t::struct_t::field_t>::iterator const end)
{
    for (; it != end; ++it)
    {
        auto& field = *it;
        impl::substitute(var, y, field.type);
        if (field.var == var)
        {
            // `field.var` is now a new binding type-variable;
            // any later fields refer to `field.var` not to the initial `var`;
            // so substitution must stop, including for the return type
            // NB this cannot really happen because field names cannot repeat in a struct; but just in case
            return;
        }
        // We need to check if `field.var` appears in `y` and, if so, rename it.
        // Technically it would suffice to check whether `field.var` appears free in `y`,
        // but we prefer to check if it appears anywhere (i.e. also as binding variable)
        // because it renames possibly confusing types like `(typename t) -> (typename t) -> t` to
        // `(typename t:1) -> (typename t) -> t`, making it obvious to see which `t` is binding.
        // Also note that we are modifying the elements of the very vector we are iterating on,
        // but we are only modifying the values, not the vector; so iteration is safe.
        if (ast::occurs_in(field.var, y, ast::occurrence_style::anywhere))
            field.var = ast::rename<properties_t>(field.var, std::next(it), end);
    }
}

} // namespace dep0::typecheck
