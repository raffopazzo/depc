#include "dep0/typecheck/beta_delta_reduction.hpp"

#include "private/beta_reduction.hpp"
#include "private/delta_reduction.hpp"
#include "private/derivation_rules.hpp"

#include "dep0/match.hpp"

#include <cassert>

namespace dep0::typecheck {

bool beta_delta_normalize(module_t& m)
{
    environment_t env;
    bool changed = beta_normalize(m);
    for (auto& x: m.entries)
        // normalize first and then store in env, so future look-ups will find the normalized term
        changed |= match(
            x,
            [&] (type_def_t const& def)
            {
                // nothing to normalize but still need to add the definition in the environment
                auto const& name = match(def.value, [&] (auto const& x) { return x.name; });
                auto const ok = env.try_emplace(expr_t::global_t{name}, def);
                assert(ok.has_value());
                return false;
            },
            [&] (func_decl_t& decl)
            {
                bool const result = beta_delta_normalize(env, decl);
                auto const ok = env.try_emplace(expr_t::global_t{decl.name}, decl);
                assert(ok.has_value());
                return result;
            },
            [&] (func_def_t& def)
            {
                bool const result = beta_delta_normalize(env, def);
                auto const ok = env.try_emplace(expr_t::global_t{def.name}, def);
                assert(ok.has_value());
                return result;
            });
    return changed;
}

bool beta_delta_normalize(environment_t const& env, func_decl_t& decl)
{
    context_t ctx;
    bool changed = false;
    for (auto& arg: decl.signature.args)
    {
        changed |= beta_delta_normalize(env, ctx, arg.type);
        if (arg.var)
        {
            auto const ok = ctx.try_emplace(*arg.var, arg.properties.origin, make_legal_expr(arg.type, *arg.var));
            assert(ok.has_value());
        }
    }
    changed |= beta_delta_normalize(env, ctx, decl.signature.ret_type.get());
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
            auto const ok = ctx.try_emplace(*arg.var, arg.properties.origin, make_legal_expr(arg.type, *arg.var));
            assert(ok.has_value());
        }
    }
    changed |= beta_delta_normalize(env, ctx, def.value.ret_type.get());
    changed |= beta_delta_normalize(env, ctx, def.value.body);
    return changed;
}

bool beta_delta_normalize(environment_t const& env, context_t const& ctx, body_t& body)
{
    bool changed = beta_normalize(body);
    while (delta_reduce(env, ctx, body))
    {
        changed = true;
        beta_normalize(body);
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
