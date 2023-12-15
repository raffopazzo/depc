#pragma once

#include "dep0/ast/substitute.hpp"

#include "dep0/ast/occurs_in.hpp"
#include "dep0/ast/rename.hpp"

#include "dep0/match.hpp"

#include <algorithm>

namespace dep0::ast {

namespace impl {

template <Properties P>
void substitute(typename expr_t<P>::var_t const&, expr_t<P> const&, body_t<P>&);

template <Properties P>
void substitute(typename expr_t<P>::var_t const&, expr_t<P> const&, expr_t<P>&);

template <Properties P>
void substitute(typename expr_t<P>::var_t const&, expr_t<P> const&, typename expr_t<P>::app_t&);

template <Properties P>
void substitute(typename expr_t<P>::var_t const& var, expr_t<P> const& expr, body_t<P>& body)
{
    for (auto& stmt: body.stmts)
        match(
            stmt.value,
            [&] (typename expr_t<P>::app_t& app)
            {
                substitute(var, expr, app);
            },
            [&] (typename stmt_t<P>::if_else_t& if_)
            {
                substitute(var, expr, if_.cond);
                substitute(var, expr, if_.true_branch);
                if (if_.false_branch)
                    substitute(var, expr, *if_.false_branch);
            },
            [&] (typename stmt_t<P>::return_t& ret)
            {
                if (ret.expr)
                    substitute(var, expr, *ret.expr);
            });
}

template <Properties P>
void substitute(typename expr_t<P>::var_t const& var, expr_t<P> const& expr, expr_t<P>& x)
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
                    substitute(var, expr, x.lhs.get());
                    substitute(var, expr, x.rhs.get());
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
            substitute(var, expr, x);
        },
        [&] (typename expr_t<P>::abs_t& x)
        {
            substitute(var, expr, x.args.begin(), x.args.end(), x.ret_type.get(), &x.body);
        },
        [&] (typename expr_t<P>::pi_t& x)
        {
            substitute<P>(var, expr, x.args.begin(), x.args.end(), x.ret_type.get(), nullptr);
        },
        [&] (typename expr_t<P>::array_t& x)
        {
            substitute(var, expr, x.type.get());
            substitute(var, expr, x.size.get());
        },
        [&] (typename expr_t<P>::init_list_t& x)
        {
            for (auto& v: x.values)
                substitute(var, expr, v);
        });
}

template <Properties P>
void substitute(typename expr_t<P>::var_t const& var, expr_t<P> const& expr, typename expr_t<P>::app_t& app)
{
    substitute(var, expr, app.func.get());
    for (auto& arg: app.args)
        substitute(var, expr, arg);
}

} // namespace impl

template <Properties P>
void substitute(
    typename expr_t<P>::var_t const& var,
    expr_t<P> const& y,
    typename std::vector<func_arg_t<P>>::iterator it,
    typename std::vector<func_arg_t<P>>::iterator const end,
    expr_t<P>& ret_type,
    body_t<P>* body)
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
        // but we are only modifying the values, no the vector; so iteration is safe.
        if (arg.var and occurs_in(*arg.var, y, occurrence_style::anywhere))
            arg.var = rename<P>(*arg.var, std::next(it), end, ret_type, body);
    }
    impl::substitute(var, y, ret_type);
    if (body)
        impl::substitute(var, y, *body);
}

} // namespace dep0::ast
