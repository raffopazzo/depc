#include "private/substitute.hpp"

#include "dep0/match.hpp"

namespace dep0::transform {

static void substitute(typecheck::stmt_t&, typecheck::expr_t::var_t const&, typecheck::expr_t const&);
static void substitute(typecheck::stmt_t::if_else_t&, typecheck::expr_t::var_t const&, typecheck::expr_t const&);
static void substitute(typecheck::stmt_t::return_t&, typecheck::expr_t::var_t const&, typecheck::expr_t const&);
static void substitute(typecheck::expr_t::app_t&, typecheck::expr_t::var_t const&, typecheck::expr_t const&);

// substitute

void substitute(typecheck::body_t& body, typecheck::expr_t::var_t const& var, typecheck::expr_t const& expr)
{
    for (auto& stmt: body.stmts)
        substitute(stmt, var, expr);
}

void substitute(typecheck::stmt_t& stmt, typecheck::expr_t::var_t const& var, typecheck::expr_t const& expr)
{
    match(stmt.value, [&] (auto& x) { substitute(x, var, expr); });
}

void substitute(typecheck::stmt_t::if_else_t& if_, typecheck::expr_t::var_t const& var, typecheck::expr_t const& expr)
{
    substitute(if_.cond, var, expr);
    substitute(if_.true_branch, var, expr);
    if (if_.false_branch)
        substitute(*if_.false_branch, var, expr);
}

void substitute(typecheck::stmt_t::return_t& ret, typecheck::expr_t::var_t const& var, typecheck::expr_t const& expr)
{
    if (ret.expr)
        substitute(*ret.expr, var, expr);
}

void substitute(typecheck::expr_t& x, typecheck::expr_t::var_t const& var, typecheck::expr_t const& expr)
{
    match(
        x.value,
        [&] (typecheck::expr_t::arith_expr_t& x)
        {
            match(
                x.value,
                [&] (typecheck::expr_t::arith_expr_t::plus_t& x)
                {
                    substitute(x.lhs.get(), var, expr);
                    substitute(x.rhs.get(), var, expr);
                });
        },
        [&] (typecheck::expr_t::boolean_constant_t&) { },
        [&] (typecheck::expr_t::numeric_constant_t&) { },
        [&] (typecheck::expr_t::var_t& v)
        {
            if (v == var)
                x = expr;
        },
        [&] (typecheck::expr_t::app_t& x)
        {
            substitute(x, var, expr);
        },
        [&] (typecheck::expr_t::abs_t& abs)
        {
            // only free occurrences of `var` can be substituted;
            // so if any binding variable is `var` then we cannot perform substitution
            for (auto const& arg: abs.args)
                if (arg.name == var.name)
                    return;
            // TODO if any `arg` of the abstraction appears as a free in `expr`, we need to rename `arg`
            // TODO with dependent types we will need to perform substitution in `ret_type`
            // TODO say you capture a variable by ref, then it's free; is it correct to substitute it?
            substitute(abs.body, var, expr);
        },
        [&] (typecheck::type_t&) { });
}

void substitute(typecheck::expr_t::app_t& app, typecheck::expr_t::var_t const& var, typecheck::expr_t const& expr)
{
    substitute(app.func.get(), var, expr);
    for (auto& arg: app.args)
        substitute(arg, var, expr);
}

} // namespace dep0::transform

