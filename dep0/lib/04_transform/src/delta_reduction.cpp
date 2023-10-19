#include "dep0/transform/delta_reduction.hpp"

#include "dep0/match.hpp"
#include "dep0/scope_map.hpp"

#include <cassert>
#include <ranges>

namespace dep0::transform {

using context_t = delta_reduction::context_t;

static bool delta_reduce(context_t const&, typecheck::stmt_t::if_else_t&);
static bool delta_reduce(context_t const&, typecheck::stmt_t::return_t&);
static bool delta_reduce(context_t const&, typecheck::expr_t::arith_expr_t&);
static bool delta_reduce(context_t const&, typecheck::expr_t::boolean_constant_t&);
static bool delta_reduce(context_t const&, typecheck::expr_t::numeric_constant_t&);
static bool delta_reduce(context_t const&, typecheck::expr_t::var_t&);
static bool delta_reduce(context_t const&, typecheck::expr_t::app_t&);
static bool delta_reduce(context_t const&, typecheck::expr_t::abs_t&);

bool delta_reduce(context_t const& ctx, typecheck::func_def_t& def)
{
    return delta_reduce(ctx, def.value);
}

bool delta_reduce(context_t const& ctx, typecheck::body_t& body)
{
    for (auto& stmt: body.stmts)
        if (delta_reduce(ctx, stmt))
            return true;
    return false;
}

bool delta_reduce(context_t const& ctx, typecheck::stmt_t& stmt)
{
    return match(stmt.value, [&] (auto& x) { return delta_reduce(ctx, x); });
}

bool delta_reduce(context_t const& ctx, typecheck::stmt_t::if_else_t& if_)
{
    return delta_reduce(ctx, if_.cond)
        or delta_reduce(ctx, if_.true_branch)
        or if_.false_branch and delta_reduce(ctx, *if_.false_branch);
}

bool delta_reduce(context_t const& ctx, typecheck::stmt_t::return_t& ret)
{
    return ret.expr and delta_reduce(ctx, *ret.expr);
}

bool delta_reduce(context_t const& ctx, typecheck::expr_t& expr)
{
    return match(
        expr.value,
        [&] (typecheck::expr_t::var_t& var)
        {
            // during expansion of top-level function definitions,
            // the current function is not in context to avoid infinite recursion
            if (auto const val = ctx[var])
                if (auto const abs = std::get_if<typecheck::expr_t::abs_t>(val))
                {
                    expr.value = *abs;
                    return true;
                }
            return false;
        },
        [&] (auto& x) { return delta_reduce(ctx, x); });
}

bool delta_reduce(context_t const& ctx, typecheck::expr_t::arith_expr_t& x)
{
    return match(
        x.value,
        [&] (typecheck::expr_t::arith_expr_t::plus_t& x)
        {
            return delta_reduce(ctx, x.lhs.get()) or delta_reduce(ctx, x.rhs.get());
        });
}

bool delta_reduce(context_t const& ctx, typecheck::expr_t::boolean_constant_t&) { return false; }
bool delta_reduce(context_t const& ctx, typecheck::expr_t::numeric_constant_t&) { return false; }

bool delta_reduce(context_t const& ctx, typecheck::expr_t::app_t& app)
{
    if (delta_reduce(ctx, app.func.get()))
        return true;
    for (auto& arg: app.args)
        if (delta_reduce(ctx, arg))
            return true;
    return false;
}

bool delta_reduce(context_t const& ctx, typecheck::expr_t::abs_t& abs)
{
    auto ctx2 = ctx.extend();
    for (auto const& arg: abs.args)
        ctx2.try_emplace(arg.var, delta_reduction::something_else_t{});
    return delta_reduce(ctx2, abs.body);
}

bool delta_reduce(context_t const& ctx, typecheck::type_t&) { return false; }

} // namespace dep0::transform

