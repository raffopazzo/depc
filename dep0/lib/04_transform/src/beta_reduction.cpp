#include "dep0/transform/beta_reduction.hpp"

#include "dep0/match.hpp"

#include <algorithm>
#include <cassert>
#include <ranges>

template <typename T>
void destructive_self_assign(T& x, T&& y)
{
    auto tmp = std::move(y);
    x = std::move(tmp);
}

namespace dep0::transform {

static bool beta_normalize(typecheck::stmt_t::if_else_t&);
static bool beta_normalize(typecheck::stmt_t::return_t&);
static bool beta_normalize(typecheck::expr_t::arith_expr_t&);
static bool beta_normalize(typecheck::expr_t::boolean_constant_t&);
static bool beta_normalize(typecheck::expr_t::numeric_constant_t&);
static bool beta_normalize(typecheck::expr_t::var_t&);
static bool beta_normalize(typecheck::expr_t::app_t&);
static bool beta_normalize(typecheck::expr_t::abs_t&);

static void substitute(typecheck::body_t&, typecheck::expr_t::var_t const&, typecheck::expr_t const&);
static void substitute(typecheck::stmt_t&, typecheck::expr_t::var_t const&, typecheck::expr_t const&);
static void substitute(typecheck::stmt_t::if_else_t&, typecheck::expr_t::var_t const&, typecheck::expr_t const&);
static void substitute(typecheck::stmt_t::return_t&, typecheck::expr_t::var_t const&, typecheck::expr_t const&);
static void substitute(typecheck::expr_t&, typecheck::expr_t::var_t const&, typecheck::expr_t const&);
static void substitute(typecheck::expr_t::app_t&, typecheck::expr_t::var_t const&, typecheck::expr_t const&);

bool beta_normalize(typecheck::module_t& m)
{
    bool changed = false;
    for (auto& def: m.func_defs)
        changed |= beta_normalize(def);
    return changed;
}

bool beta_normalize(typecheck::func_def_t& def)
{
    return beta_normalize(def.value.body);
}

bool beta_normalize(typecheck::body_t& body)
{
    auto const is_return = [] (typecheck::stmt_t const& x)
    {
        return std::holds_alternative<typecheck::stmt_t::return_t>(x.value);
    };
    // NB taking `stmts` by value because we are about to perform a destructive self-assignment
    auto replace_with = [&] (std::vector<typecheck::stmt_t>::iterator it, std::vector<typecheck::stmt_t> stmts)
    {
        // If there is nothing to replace `it` with, then just remove it;
        // otherwise we need to splice all the replacing statements in place of `it`,
        // which can be done efficiently by replacing `it` with the first statement, and insert the rest after it.
        // Finally, if the new statements contain a return statment, any other statement after that can be dropped.
        if (stmts.empty())
            return body.stmts.erase(it);
        auto const index_of_ret = [&]
        {
            auto const j = std::ranges::find_if(stmts, is_return);
            return j == stmts.end() ? std::nullopt : std::optional{std::distance(stmts.begin(), j)};
        }();
        auto j = stmts.begin();
        *it++ = std::move(*j++);
        it = body.stmts.insert(it, std::make_move_iterator(j), std::make_move_iterator(stmts.end()));
        if (index_of_ret)
            // if you were expecting a `+1` here, the reason it's missing is because we manually incremented `it` once
            body.stmts.erase(it + *index_of_ret, body.stmts.end());
        return it;
    };
    bool changed = false;
    auto it = body.stmts.begin();
    while (it != body.stmts.end())
    {
        changed |= beta_normalize(*it);
        it = match(
            it->value,
            [&] (typecheck::expr_t::app_t& app)
            {
                // if immediately-invoked lambda we can extract the body,
                // but have to suppress return statements from the invoked lambda,
                // otherwise they would become early returns from the parent function
                // TODO blindly suppressing return of the invoked lambda will be wrong once we have side-effects
                if (app.args.empty())
                    if (auto* const abs = std::get_if<typecheck::expr_t::abs_t>(&app.func.get().value))
                    {
                        changed = true;
                        std::erase_if(abs->body.stmts, is_return);
                        return replace_with(it, std::move(abs->body.stmts));
                    }
                return std::next(it);
            },
            [&] (typecheck::stmt_t::if_else_t& if_)
            {
                if (auto const c = std::get_if<typecheck::expr_t::boolean_constant_t>(&if_.cond.value))
                {
                    changed = true;
                    if (c->value == "true")
                        return replace_with(it, std::move(if_.true_branch.stmts));
                    else if (if_.false_branch)
                        return replace_with(it, std::move(if_.false_branch->stmts));
                    else
                        return body.stmts.erase(it);
                }
                else
                    return std::next(it);
            },
            [&] (typecheck::stmt_t::return_t const&) { return std::next(it); });
    }
    return changed;
}

bool beta_normalize(typecheck::stmt_t& stmt)
{
    return match(stmt.value, [&] (auto& x) { return beta_normalize(x); });
}

bool beta_normalize(typecheck::stmt_t::if_else_t& if_)
{
    bool changed = beta_normalize(if_.cond);
    changed |= beta_normalize(if_.true_branch);
    if (if_.false_branch)
        changed |= beta_normalize(*if_.false_branch);
    return changed;
}

bool beta_normalize(typecheck::stmt_t::return_t& ret)
{
    return ret.expr and beta_normalize(*ret.expr);
}

bool beta_normalize(typecheck::expr_t& expr)
{
    return match(
        expr.value,
        [&] (typecheck::expr_t::app_t& app)
        {
            bool changed = beta_normalize(app);
            // if this reduced to a parameterless single return statement, we can extract the returned expression
            if (app.args.empty())
                if (auto* const abs = std::get_if<typecheck::expr_t::abs_t>(&app.func.get().value))
                    if (abs->body.stmts.size() == 1ul)
                        if (auto* const ret = std::get_if<typecheck::stmt_t::return_t>(&abs->body.stmts[0].value))
                            if (ret->expr)
                            {
                                changed = true;
                                destructive_self_assign(expr, std::move(*ret->expr));
                            }
            return changed;
        },
        [&] (auto& x) { return beta_normalize(x); });
}

bool beta_normalize(typecheck::expr_t::arith_expr_t& x)
{
    // TODO constant folding?
    return match(
        x.value,
        [&] (typecheck::expr_t::arith_expr_t::plus_t& x)
        {
            bool changed = beta_normalize(x.lhs.get());
            changed |= beta_normalize(x.rhs.get());
            return changed;
        });
}

bool beta_normalize(typecheck::expr_t::boolean_constant_t&) { return false; }
bool beta_normalize(typecheck::expr_t::numeric_constant_t&) { return false; }
bool beta_normalize(typecheck::expr_t::var_t&) { return false; }

bool beta_normalize(typecheck::expr_t::app_t& app)
{
    bool changed = beta_normalize(app.func.get());
    for (auto& arg: app.args)
        changed |= beta_normalize(arg);
    if (auto* const abs = std::get_if<typecheck::expr_t::abs_t>(&app.func.get().value))
    {
        changed |= abs->args.size() > 0ul; // because we are about to remove the arguments
        assert(abs->args.size() == app.args.size());
        for (auto const i: std::views::iota(0ul, abs->args.size()))
            substitute(abs->body, typecheck::expr_t::var_t{abs->args[i].name}, app.args[i]);
        // at this point all arguments of the abstraction have been substituted,
        // so we can remove them and normalize the new body
        app.args.clear();
        abs->args.clear();
        changed |= beta_normalize(abs->body);
    }
    return changed;
}

bool beta_normalize(typecheck::expr_t::abs_t& abs)
{
    return beta_normalize(abs.body);
}

bool beta_normalize(typecheck::type_t&) { return false; }

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
