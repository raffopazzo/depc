#pragma once

#include "dep0/ast/substitute.hpp"

#include "dep0/ast/contains_var.hpp"
#include "dep0/ast/rename.hpp"

#include "dep0/match.hpp"

#include <algorithm>

namespace dep0::ast {

template <Properties P>
void substitute(typename expr_t<P>::app_t&, typename expr_t<P>::var_t const&, expr_t<P> const&);

template <Properties P>
void substitute(stmt_t<P>&, typename expr_t<P>::var_t const&, expr_t<P> const&);

template <Properties P>
void substitute(
    typename expr_t<P>::abs_t::arg_iterator it,
    typename expr_t<P>::abs_t::arg_iterator const end,
    expr_t<P>& ret_type,
    typename expr_t<P>::var_t const& var,
    expr_t<P> const& y)
{
    for (; it != end; ++it)
    {
        auto& arg = *it;
        substitute(arg.type, var, y);
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
        // but we are only modifying the values, no the vector; so iteration is safe.
        if (arg.var and contains_var(y, *arg.var, occurrence_style::anywhere))
            arg.var = rename(*arg.var, std::next(it), end, ret_type);
    }
    substitute(ret_type, var, y);
}

template <Properties P>
void substitute(expr_t<P>& x, typename expr_t<P>::var_t const& var, expr_t<P> const& expr)
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
        [&] (typename expr_t<P>::abs_t& x)
        {
            // TODO shouldn't we substitute inside the argument types, at least untile the first rebiding?
            // if any argument introduces a new binding variable, substitution cannot occur
            if (std::ranges::any_of(x.args, [&] (func_arg_t<P> const& arg) { return arg.var == var; }))
                return;
            substitute(x.args.begin(), x.args.end(), x.ret_type.get(), var, expr);
            substitute(x.body, var, expr);
        },
        [&] (typename expr_t<P>::pi_t& x)
        {
            substitute(x.args.begin(), x.args.end(), x.ret_type.get(), var, expr);
        });
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
