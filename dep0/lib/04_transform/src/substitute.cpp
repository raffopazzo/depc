#include "private/substitute.hpp"

#include "dep0/match.hpp"

#include <algorithm>
#include <numeric>

namespace dep0::transform {

static void substitute(typecheck::body_t&, typecheck::expr_t::var_t const&, typecheck::expr_t const&);
static void substitute(typecheck::expr_t::app_t&, typecheck::expr_t::var_t const&, typecheck::expr_t const&);
static void substitute(typecheck::stmt_t&, typecheck::expr_t::var_t const&, typecheck::expr_t const&);

static bool contains_var(typecheck::body_t const&, typecheck::expr_t::var_t const&);
static bool contains_var(typecheck::expr_t const&, typecheck::expr_t::var_t const&);
static bool contains_var(typecheck::expr_t::app_t const&, typecheck::expr_t::var_t const&);
static std::size_t max_index(typecheck::body_t const&);
static std::size_t max_index(typecheck::expr_t const&);
static std::size_t max_index(typecheck::expr_t::app_t const&);
static typecheck::expr_t::var_t rename(typecheck::expr_t::var_t const&, typecheck::body_t&);
static void replace(typecheck::expr_t::var_t const&, typecheck::expr_t::var_t const&, typecheck::body_t&);
static void replace(typecheck::expr_t::var_t const&, typecheck::expr_t::var_t const&, typecheck::expr_t&);
static void replace(typecheck::expr_t::var_t const&, typecheck::expr_t::var_t const&, typecheck::expr_t::app_t&);

// substitute
void substitute(
    typecheck::expr_t::abs_t::arg_iterator const begin,
    typecheck::expr_t::abs_t::arg_iterator const end,
    typecheck::body_t& body,
    typecheck::expr_t::var_t const& var,
    typecheck::expr_t const& expr)
{
    // only free occurrences of `var` can be substituted;
    // so if any binding variable is `var` then we cannot perform substitution
    for (auto const& arg: std::ranges::subrange(begin, end))
        if (arg.var == var)
            return;
    // if any `arg` appears free in `expr`, we need to rename `arg`
    for (auto& arg: std::ranges::subrange(begin, end))
        if (contains_var(expr, arg.var))
            // TODO should consider all args before renaming (i.e. for max index)
            arg.var = rename(arg.var, body);
    // TODO with dependent types we will need to perform substitution (and renaming) in `ret_type` too
    // TODO bound variables inside lambda captures must not be substituted; imagine this
    // `i32_t g(i32_t x) { auto f = [x=1] { return x; }; return f() + x; }` and imagine the application
    // `g(2)`; during substitution of x:=2, the x inside f must not be substituted; this is effectively
    // equivalent to `i32_t g(i32_t x) { auto f = (x) { return x; }; return f(1) + x; }`, in which
    // substitution does not occur because f introduces a shadowing variable x.
    substitute(body, var, expr);
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
            substitute(abs.args.begin(), abs.args.end(), abs.body, var, expr);
        },
        [&] (typecheck::type_t&)
        {
            // TODO either implement this or explain why it's not necessary
        });
}

void substitute(typecheck::body_t& body, typecheck::expr_t::var_t const& var, typecheck::expr_t const& expr)
{
    for (auto& stmt: body.stmts)
        substitute(stmt, var, expr);
}

void substitute(typecheck::expr_t::app_t& app, typecheck::expr_t::var_t const& var, typecheck::expr_t const& expr)
{
    substitute(app.func.get(), var, expr);
    for (auto& arg: app.args)
        substitute(arg, var, expr);
}

void substitute(typecheck::stmt_t& stmt, typecheck::expr_t::var_t const& var, typecheck::expr_t const& expr)
{
    match(
        stmt.value,
        [&] (typecheck::expr_t::app_t& app)
        {
            substitute(app, var, expr);
        },
        [&] (typecheck::stmt_t::if_else_t& if_)
        {
            substitute(if_.cond, var, expr);
            substitute(if_.true_branch, var, expr);
            if (if_.false_branch)
                substitute(*if_.false_branch, var, expr);
        },
        [&] (typecheck::stmt_t::return_t& ret)
        {
            if (ret.expr)
                substitute(*ret.expr, var, expr);
        });
}

bool contains_var(typecheck::body_t const& x, typecheck::expr_t::var_t const& var)
{
    return std::ranges::any_of(
        x.stmts,
        [&] (typecheck::stmt_t const& stmt)
        {
            return match(
                stmt.value,
                [&] (typecheck::expr_t::app_t const& app)
                {
                    return contains_var(app, var);
                },
                [&] (typecheck::stmt_t::if_else_t const& if_)
                {
                    return contains_var(if_.cond, var)
                        or contains_var(if_.true_branch, var)
                        or if_.false_branch and contains_var(*if_.false_branch, var);
                },
                [&] (typecheck::stmt_t::return_t const& ret)
                {
                    return ret.expr and contains_var(*ret.expr, var);
                });
        });
}

bool contains_var(typecheck::expr_t const& x, typecheck::expr_t::var_t const& var)
{
    return match(
        x.value,
        [&] (typecheck::expr_t::arith_expr_t const& x)
        {
            return match(
                x.value,
                [&] (typecheck::expr_t::arith_expr_t::plus_t const& x)
                {
                    return contains_var(x.lhs.get(), var) or contains_var(x.rhs.get(), var);
                });
        },
        [&] (typecheck::expr_t::boolean_constant_t const&) { return false; },
        [&] (typecheck::expr_t::numeric_constant_t const&) { return false; },
        [&] (typecheck::expr_t::var_t const& x)
        {
            return x == var;
        },
        [&] (typecheck::expr_t::app_t const& x)
        {
            return contains_var(x, var);
        },
        [&] (typecheck::expr_t::abs_t const& x)
        {
            return std::ranges::any_of(x.args, [&] (auto const& arg) { return arg.var == var; })
                or contains_var(x.body, var);
        },
        [&] (typecheck::type_t const& x)
        {
            // TODO either implement this or explain why it's not necessary
            return false;
        });
}

bool contains_var(typecheck::expr_t::app_t const& x, typecheck::expr_t::var_t const& var)
{
    return contains_var(x.func.get(), var)
        or std::ranges::any_of(x.args, [&] (typecheck::expr_t const& arg) { return contains_var(arg, var); });
}

static typecheck::expr_t::var_t rename(typecheck::expr_t::var_t const& var, typecheck::body_t& body)
{
    auto const max_idx = std::max(var.name.idx, max_index(body));
    auto const new_var = typecheck::expr_t::var_t{ast::indexed_var_t{var.name.txt, max_idx + 1ul}};
    replace(var, new_var, body);
    return new_var;
}

std::size_t max_index(typecheck::body_t const& x)
{
    return std::accumulate(
        x.stmts.begin(), x.stmts.end(),
        0ul,
        [] (std::size_t const acc, typecheck::stmt_t const& s)
        {
            return std::max(
                acc,
                match(
                    s.value,
                    [] (typecheck::expr_t::app_t const& x)
                    {
                        return max_index(x);
                    },
                    [] (typecheck::stmt_t::if_else_t const& if_)
                    {
                        return std::max(
                            max_index(if_.cond),
                            std::max(
                                max_index(if_.true_branch),
                                if_.false_branch ? max_index(*if_.false_branch) : 0ul));
                    },
                    [] (typecheck::stmt_t::return_t const& ret)
                    {
                        return ret.expr ? max_index(*ret.expr) : 0ul;
                    }));
        });
}

std::size_t max_index(typecheck::expr_t const& x)
{
    return match(
        x.value,
        [&] (typecheck::expr_t::arith_expr_t const& x)
        {
            return match(
                x.value,
                [&] (typecheck::expr_t::arith_expr_t::plus_t const& x)
                {
                    return std::max(max_index(x.lhs.get()), max_index(x.rhs.get()));
                });
        },
        [&] (typecheck::expr_t::boolean_constant_t const&) { return 0ul; },
        [&] (typecheck::expr_t::numeric_constant_t const&) { return 0ul; },
        [&] (typecheck::expr_t::var_t const& x)
        {
            return x.name.idx;
        },
        [&] (typecheck::expr_t::app_t const& x)
        {
            return max_index(x);
        },
        [&] (typecheck::expr_t::abs_t const& x)
        {
            return std::accumulate(
                x.args.begin(), x.args.end(),
                max_index(x.body),
                [] (std::size_t const acc, typecheck::expr_t::abs_t::arg_t const& arg)
                {
                    return std::max(acc, arg.var.name.idx);
                });
        },
        [&] (typecheck::type_t const& x)
        {
            // TODO either implement this or explain why it's not necessary
            return 0ul;
        });
}

std::size_t max_index(typecheck::expr_t::app_t const& x)
{
    return std::accumulate(
        x.args.begin(), x.args.end(),
        max_index(x.func.get()),
        [] (std::size_t const acc, typecheck::expr_t const& arg)
        {
            return std::max(acc, max_index(arg));
        });
}

void replace(typecheck::expr_t::var_t const& from, typecheck::expr_t::var_t const& to, typecheck::body_t& x)
{
    for (auto& s: x.stmts)
        match(
            s.value,
            [&] (typecheck::expr_t::app_t& app)
            {
                return replace(from, to, app);
            },
            [&] (typecheck::stmt_t::if_else_t& if_)
            {
                replace(from, to, if_.cond);
                replace(from, to, if_.true_branch);
                if (if_.false_branch)
                    replace(from, to, *if_.false_branch);
            },
            [&] (typecheck::stmt_t::return_t& ret)
            {
                if (ret.expr)
                    replace(from, to, *ret.expr);
            });
}

void replace(typecheck::expr_t::var_t const& from, typecheck::expr_t::var_t const& to, typecheck::expr_t& x)
{
    match(
        x.value,
        [&] (typecheck::expr_t::arith_expr_t& x)
        {
            return match(
                x.value,
                [&] (typecheck::expr_t::arith_expr_t::plus_t& x)
                {
                    replace(from, to, x.lhs.get());
                    replace(from, to, x.rhs.get());
                });
        },
        [&] (typecheck::expr_t::boolean_constant_t const&) { },
        [&] (typecheck::expr_t::numeric_constant_t const&) { },
        [&] (typecheck::expr_t::var_t& v)
        {
            if (v == from)
                v = to;
        },
        [&] (typecheck::expr_t::app_t& app)
        {
            return replace(from, to, app);
        },
        [&] (typecheck::expr_t::abs_t& x)
        {
            // Note: in theory it would suffice to replace only the free occurrences of `from`
            // and we could stop if `from` introduces a new binding variable;
            // but replacing everything is easier and it shouldn't harm.
            for (auto& arg: x.args)
                if (arg.var == from)
                    arg.var = to;
            replace(from, to, x.body);
        },
        [&] (typecheck::type_t const& x)
        {
            // TODO either implement this or explain why it's not necessary
        });
}

void replace(typecheck::expr_t::var_t const& from, typecheck::expr_t::var_t const& to, typecheck::expr_t::app_t& x)
{
    replace(from, to, x.func.get());
    for (auto& arg: x.args)
        replace(from, to, arg);
}

} // namespace dep0::transform

