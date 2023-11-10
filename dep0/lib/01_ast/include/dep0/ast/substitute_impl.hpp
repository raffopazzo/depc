#pragma once

#include "dep0/ast/substitute.hpp"

#include "dep0/ast/contains_var.hpp"
#include "dep0/ast/rename.hpp"

#include "dep0/match.hpp"

#include <ranges>

namespace dep0::ast {

template <Properties P>
void substitute(body_t<P>&, typename expr_t<P>::var_t const&, expr_t<P> const&);

template <Properties P>
void substitute(typename expr_t<P>::app_t&, typename expr_t<P>::var_t const&, expr_t<P> const&);

template <Properties P>
void substitute(stmt_t<P>&, typename expr_t<P>::var_t const&, expr_t<P> const&);

template <Properties P>
void substitute(type_t<P>& x, typename type_t<P>::var_t const& var, type_t<P> const& y)
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
        [&] (typename type_t<P>::var_t const& v)
        {
            if (v == var)
                x = y;
        },
        [&] (typename type_t<P>::arr_t& arr)
        {
            substitute(arr.args.begin(), arr.args.end(), arr.ret_type.get(), var, y);
        });
}

template <Properties P>
void substitute(
    typename type_t<P>::arr_t::arg_iterator it,
    typename type_t<P>::arr_t::arg_iterator const end,
    sort_t<P>& ret_type,
    typename type_t<P>::var_t const& var,
    type_t<P> const& y)
{
    for (; it != end; ++it)
    {
        bool const stop = match(
            it->value,
            [&] (typename func_arg_t<P>::type_arg_t& arg)
            {
                if (arg.var == var)
                {
                    // `arg.var` is now a new binding type-variable;
                    // any later arguments refer to `arg.var` not to the initial `var`;
                    // so substitution must stop, including for the return type
                    return true;
                }
                // We need to check if `arg.var` appears free in `y` and, if so, rename it.
                // Technically here we are actually checking whether `arg.var` appears at all in `y`, i.e.
                // not only free but also as binding variable; this is not strictly necessary,
                // but the code is simpler and it also avoids possibly confusing types like this
                // `(typename t) -> (typename t) -> t` which instead become `(typename t:1) -> (typename t) -> t`,
                // making it more obvious which `t` is binding, so for now let's go with this.
                // Also note that we are modifying the elements of the very vector we are iterating on,
                // but we are only modifying the values, no the vector; so iteration is safe.
                if (arg.var and contains_var(y, *arg.var))
                    arg.var = rename(*arg.var, std::next(it), end, ret_type);
                return false;
            },
            [&] (typename func_arg_t<P>::term_arg_t& arg)
            {
                substitute(arg.type, var, y);
                return false;
            });
        if (stop)
            return;
    }
    if (auto const t = std::get_if<type_t<P>>(&ret_type))
        substitute(*t, var, y);
}

template <Properties P>
void substitute(
    typename expr_t<P>::abs_t::arg_iterator const begin,
    typename expr_t<P>::abs_t::arg_iterator const end,
    body_t<P>& body,
    typename expr_t<P>::var_t const& var,
    expr_t<P> const& expr)
{
    // only free occurrences of `var` can be substituted;
    // so if any binding variable is `var` then we cannot perform substitution
    for (auto const& arg: std::ranges::subrange(begin, end))
    {
        bool const stop =
            match(
                arg.value,
                [] (typename func_arg_t<P>::type_arg_t const&) { return false; },
                [&] (typename func_arg_t<P>::term_arg_t const& term_arg) { return term_arg.var == var; });
        if (stop)
            return;
    }
    // if any `arg` appears free in `expr`, we need to rename `arg`
    for (auto& arg: std::ranges::subrange(begin, end))
        match(
            arg.value,
            [] (typename func_arg_t<P>::type_arg_t const&) { },
            [&] (typename func_arg_t<P>::term_arg_t& term_arg)
            {
                if (term_arg.var and contains_var(expr, *term_arg.var))
                    // TODO renaming should take into account all args, not just the body; probably better with a test
                    term_arg.var = rename(*term_arg.var, body);
            });
    // TODO with dependent types we will need to perform substitution (and renaming) in `ret_type` too
    // TODO bound variables inside lambda captures must not be substituted; imagine this
    // `i32_t g(i32_t x) { auto f = [x=1] { return x; }; return f() + x; }` and imagine the application
    // `g(2)`; during substitution of x:=2, the x inside f must not be substituted; this is effectively
    // equivalent to `i32_t g(i32_t x) { auto f = (x) { return x; }; return f(1) + x; }`, in which
    // substitution does not occur because f introduces a shadowing variable x.
    substitute(body, var, expr);
}

template <Properties P>
void substitute(expr_t<P>& x, typename expr_t<P>::var_t const& var, expr_t<P> const& expr)
{
    match(
        x.value,
        [&] (typename expr_t<P>::arith_expr_t& x)
        {
            match(
                x.value,
                [&] (typename expr_t<P>::arith_expr_t::plus_t& x)
                {
                    substitute(x.lhs.get(), var, expr);
                    substitute(x.rhs.get(), var, expr);
                });
        },
        [&] (typename expr_t<P>::boolean_constant_t&) { },
        [&] (typename expr_t<P>::numeric_constant_t&) { },
        [&] (typename expr_t<P>::var_t& v)
        {
            if (v == var)
                x = expr;
        },
        [&] (typename expr_t<P>::app_t& x)
        {
            substitute(x, var, expr);
        },
        [&] (typename expr_t<P>::abs_t& abs)
        {
            substitute(abs.args.begin(), abs.args.end(), abs.body, var, expr);
        },
        [&] (type_t<P> const&) { });
}

template <Properties P>
void substitute(body_t<P>& body, typename expr_t<P>::var_t const& var, expr_t<P> const& expr)
{
    for (auto& stmt: body.stmts)
        substitute(stmt, var, expr);
}

template <Properties P>
void substitute(typename expr_t<P>::app_t& app, typename expr_t<P>::var_t const& var, expr_t<P> const& expr)
{
    substitute(app.func.get(), var, expr);
    for (auto& arg: app.args)
        substitute(arg, var, expr);
}

template <Properties P>
void substitute(stmt_t<P>& stmt, typename expr_t<P>::var_t const& var, expr_t<P> const& expr)
{
    match(
        stmt.value,
        [&] (typename expr_t<P>::app_t& app)
        {
            substitute(app, var, expr);
        },
        [&] (typename stmt_t<P>::if_else_t& if_)
        {
            substitute(if_.cond, var, expr);
            substitute(if_.true_branch, var, expr);
            if (if_.false_branch)
                substitute(*if_.false_branch, var, expr);
        },
        [&] (typename stmt_t<P>::return_t& ret)
        {
            if (ret.expr)
                substitute(*ret.expr, var, expr);
        });
}

} // namespace dep0::ast
