#include "private/delta_reduction.hpp"

#include "private/derivation_rules.hpp"

#include "dep0/match.hpp"

#include <cassert>

namespace dep0::typecheck {

namespace impl {

static bool delta_reduce(environment_t const&, context_t const&, stmt_t&);
static bool delta_reduce(environment_t const&, context_t const&, stmt_t::if_else_t&);
static bool delta_reduce(environment_t const&, context_t const&, stmt_t::return_t&);
static bool delta_reduce(environment_t const&, context_t const&, expr_t::typename_t&);
static bool delta_reduce(environment_t const&, context_t const&, expr_t::bool_t&);
static bool delta_reduce(environment_t const&, context_t const&, expr_t::unit_t&);
static bool delta_reduce(environment_t const&, context_t const&, expr_t::i8_t&);
static bool delta_reduce(environment_t const&, context_t const&, expr_t::i16_t&);
static bool delta_reduce(environment_t const&, context_t const&, expr_t::i32_t&);
static bool delta_reduce(environment_t const&, context_t const&, expr_t::i64_t&);
static bool delta_reduce(environment_t const&, context_t const&, expr_t::u8_t&);
static bool delta_reduce(environment_t const&, context_t const&, expr_t::u16_t&);
static bool delta_reduce(environment_t const&, context_t const&, expr_t::u32_t&);
static bool delta_reduce(environment_t const&, context_t const&, expr_t::u64_t&);
static bool delta_reduce(environment_t const&, context_t const&, expr_t::boolean_constant_t&);
static bool delta_reduce(environment_t const&, context_t const&, expr_t::numeric_constant_t&);
static bool delta_reduce(environment_t const&, context_t const&, expr_t::boolean_expr_t&);
static bool delta_reduce(environment_t const&, context_t const&, expr_t::relation_expr_t&);
static bool delta_reduce(environment_t const&, context_t const&, expr_t::arith_expr_t&);
static bool delta_reduce(environment_t const&, context_t const&, expr_t::var_t&);
static bool delta_reduce(environment_t const&, context_t const&, expr_t::global_t&);
static bool delta_reduce(environment_t const&, context_t const&, expr_t::app_t&);
static bool delta_reduce(environment_t const&, context_t const&, expr_t::pi_t&);
static bool delta_reduce(environment_t const&, context_t const&, expr_t::array_t&);
static bool delta_reduce(environment_t const&, context_t const&, expr_t::init_list_t&);
static bool delta_reduce(environment_t const&, context_t const&, expr_t::subscript_t&);

bool delta_reduce(environment_t const& env, context_t const& ctx, stmt_t& stmt)
{
    return match(stmt.value, [&] (auto& x) { return impl::delta_reduce(env, ctx, x); });
}

bool delta_reduce(environment_t const& env, context_t const& ctx, stmt_t::if_else_t& if_)
{
    return delta_reduce(env, ctx, if_.cond)
        or delta_reduce(env, ctx, if_.true_branch)
        or if_.false_branch and delta_reduce(env, ctx, *if_.false_branch);
}

bool delta_reduce(environment_t const& env, context_t const& ctx, stmt_t::return_t& ret)
{
    return ret.expr and delta_reduce(env, ctx, *ret.expr);
}

bool delta_reduce(environment_t const&, context_t const&, expr_t::typename_t&) { return false; }
bool delta_reduce(environment_t const&, context_t const&, expr_t::bool_t&) { return false; }
bool delta_reduce(environment_t const&, context_t const&, expr_t::unit_t&) { return false; }
bool delta_reduce(environment_t const&, context_t const&, expr_t::i8_t&) { return false; }
bool delta_reduce(environment_t const&, context_t const&, expr_t::i16_t&) { return false; }
bool delta_reduce(environment_t const&, context_t const&, expr_t::i32_t&) { return false; }
bool delta_reduce(environment_t const&, context_t const&, expr_t::i64_t&) { return false; }
bool delta_reduce(environment_t const&, context_t const&, expr_t::u8_t&) { return false; }
bool delta_reduce(environment_t const&, context_t const&, expr_t::u16_t&) { return false; }
bool delta_reduce(environment_t const&, context_t const&, expr_t::u32_t&) { return false; }
bool delta_reduce(environment_t const&, context_t const&, expr_t::u64_t&) { return false; }
bool delta_reduce(environment_t const&, context_t const&, expr_t::boolean_constant_t&) { return false; }
bool delta_reduce(environment_t const&, context_t const&, expr_t::numeric_constant_t&) { return false; }

bool delta_reduce(environment_t const& env, context_t const& ctx, expr_t::boolean_expr_t& x)
{
    return match(
        x.value,
        [&] (expr_t::boolean_expr_t::not_t& x)
        {
            return delta_reduce(env, ctx, x.expr.get());
        },
        [&] (auto& x)
        {
            return delta_reduce(env, ctx, x.lhs.get()) or delta_reduce(env, ctx, x.rhs.get());
        });
}

bool delta_reduce(environment_t const& env, context_t const& ctx, expr_t::relation_expr_t& x)
{
    return match(
        x.value,
        [&] (auto& x)
        {
            return delta_reduce(env, ctx, x.lhs.get()) or delta_reduce(env, ctx, x.rhs.get());
        });
}

bool delta_reduce(environment_t const& env, context_t const& ctx, expr_t::arith_expr_t& x)
{
    return match(
        x.value,
        [&] (expr_t::arith_expr_t::plus_t& x)
        {
            return delta_reduce(env, ctx, x.lhs.get()) or delta_reduce(env, ctx, x.rhs.get());
        });
}

bool delta_reduce(environment_t const& env, context_t const&, expr_t::var_t&) { return false; }

bool delta_reduce(environment_t const& env, context_t const&, expr_t::global_t&)
{
    // We only want to perform a "simplified" one-step delta-reduction,
    // so ignore variable experssions that appear on their own.
    return false;
}

bool delta_reduce(environment_t const& env, context_t const& ctx, expr_t::app_t& app)
{
    if (auto const global = std::get_if<expr_t::global_t>(&app.func.get().value))
    {
        if (auto const func_def = std::get_if<func_def_t>(env[*global]))
        {
            app.func.get().value = func_def->value;
            return true;
        }
    }
    if (delta_reduce(env, ctx, app.func.get()))
        return true;
    for (auto& arg: app.args)
        if (delta_reduce(env, ctx, arg))
            return true;
    return false;
}

bool delta_reduce(environment_t const& env, context_t const& ctx, expr_t::abs_t& abs)
{
    auto ctx2 = ctx.extend();
    for (auto& arg: abs.args)
    {
        if (delta_reduce(env, ctx2, arg.type))
            return true;
        if (arg.var)
        {
            auto const inserted = ctx2.try_emplace(*arg.var, arg.properties.origin, make_legal_expr(arg.type, *arg.var));
            assert(inserted.has_value());
        }
    }
    if (delta_reduce(env, ctx2, abs.ret_type.get()))
        return true;
    return delta_reduce(env, ctx2, abs.body);
}

bool delta_reduce(environment_t const& env, context_t const& ctx, expr_t::pi_t& pi)
{
    auto ctx2 = ctx.extend();
    for (auto& arg: pi.args)
    {
        if (delta_reduce(env, ctx2, arg.type))
            return true;
        if (arg.var)
        {
            auto const inserted = ctx2.try_emplace(*arg.var, arg.properties.origin, make_legal_expr(arg.type, *arg.var));
            assert(inserted.has_value());
        }
    }
    return delta_reduce(env, ctx2, pi.ret_type.get());
}

bool delta_reduce(environment_t const& env, context_t const&, expr_t::array_t&)
{
    return false;
}

bool delta_reduce(environment_t const& env, context_t const& ctx, expr_t::init_list_t& init_list)
{
    for (auto& v: init_list.values)
        if (delta_reduce(env, ctx, v))
            return true;
    return false;
}

bool delta_reduce(environment_t const& env, context_t const& ctx, expr_t::subscript_t& subscript)
{
    return delta_reduce(env, ctx, subscript.array.get()) or delta_reduce(env, ctx, subscript.index.get());
}

} // namespace impl

bool delta_reduce(environment_t const& env, func_def_t& def)
{
    context_t ctx;
    return impl::delta_reduce(env, ctx, def.value);
}

bool delta_reduce(environment_t const& env, context_t const& ctx, body_t& body)
{
    for (auto& stmt: body.stmts)
        if (impl::delta_reduce(env, ctx, stmt))
            return true;
    return false;
}

bool delta_reduce(environment_t const& env, context_t const& ctx, expr_t& expr)
{
    return match(expr.value, [&] (auto& x) { return impl::delta_reduce(env, ctx, x); });
}

} // namespace dep0::typecheck
