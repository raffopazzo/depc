#include "dep0/typecheck/beta_delta_reduction.hpp"

#include "private/delta_reduction.hpp"
#include "private/derivation_rules.hpp"

#include "dep0/ast/beta_reduction.hpp"

#include "dep0/match.hpp"

#include <cassert>

namespace dep0::typecheck {

bool beta_delta_normalize(module_t& m)
{
    environment_t env;
    bool changed = beta_normalize(m);
    for (auto& def: m.func_defs)
    {
        changed |= beta_delta_normalize(env, def);
        // store the result in env, so future look-ups of this definition will find the normalized version
        auto const ok = env.try_emplace(expr_t::global_t{def.name}, std::nullopt, def);
        assert(ok.has_value());
    }
    return changed;
}

bool beta_delta_normalize(environment_t const& env, func_def_t& def)
{
    context_t ctx;
    bool changed = false;
    for (auto& arg: def.value.args)
    {
        changed |= beta_delta_normalize(env, ctx, arg.type);
        if (arg.var)
        {
            auto const ok = ctx.try_emplace(*arg.var, std::nullopt, make_legal_expr(arg.type, *arg.var));
            assert(ok.has_value());
        }
    }
    changed |= beta_delta_normalize(env, ctx, def.value.ret_type.get());
    changed |= beta_delta_normalize(env, ctx, def.value.body);
    return changed;
}

bool beta_delta_normalize(environment_t const& env, context_t const& ctx, body_t& body)
{
    bool changed = false;
    for (auto& s: body.stmts)
        changed |= beta_delta_normalize(env, ctx, s);
    return changed;
}

bool beta_delta_normalize(environment_t const& env, context_t const& ctx, stmt_t& x)
{
    bool changed = beta_normalize(x);
    while (delta_reduce(env, ctx, x))
    {
        changed = true;
        beta_normalize(x);
    }
    return changed;
}

bool beta_delta_normalize(environment_t const& env, context_t const& ctx, expr_t& x)
{
    bool changed = beta_normalize(x);
    while (delta_reduce(env, ctx, x))
    {
        changed = true;
        beta_normalize(x);
    }
    return changed;
}

} // namespace dep0::typecheck
