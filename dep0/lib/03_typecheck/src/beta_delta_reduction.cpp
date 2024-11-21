/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "dep0/typecheck/beta_delta_reduction.hpp"

#include "private/beta_reduction.hpp"
#include "private/delta_unfold.hpp"
#include "private/derivation_rules.hpp"

#include "dep0/match.hpp"

#include <cassert>

namespace dep0::typecheck {

namespace impl {

static bool beta_delta_normalize(env_t const&, axiom_t&);
static bool beta_delta_normalize(env_t const&, func_decl_t&);
static bool beta_delta_normalize(env_t const&, func_def_t&);
static bool beta_delta_normalize(env_t const&, ctx_t const&, sort_t&);
static bool beta_delta_normalize(env_t const&, ctx_t const&, body_t&);

bool beta_delta_normalize(env_t const& env, axiom_t& axiom)
{
    ctx_t ctx;
    bool changed = false;
    for (func_arg_t& arg: axiom.signature.args)
    {
        changed |= beta_delta_normalize(env, ctx, arg.type);
        auto const ok = ctx.try_emplace(arg.var, std::nullopt, ctx_t::var_decl_t{arg.qty, arg.type});
        assert(ok.has_value());
    }
    changed |= beta_delta_normalize(env, ctx, axiom.signature.ret_type.get());
    changed |= beta_delta_normalize(env, ctx, axiom.properties.sort.get());
    return changed;
}

bool beta_delta_normalize(env_t const& env, func_decl_t& decl)
{
    ctx_t ctx;
    bool changed = false;
    for (func_arg_t& arg: decl.signature.args)
    {
        changed |= beta_delta_normalize(env, ctx, arg.type);
        auto const ok = ctx.try_emplace(arg.var, std::nullopt, ctx_t::var_decl_t{arg.qty, arg.type});
        assert(ok.has_value());
    }
    changed |= beta_delta_normalize(env, ctx, decl.signature.ret_type.get());
    changed |= beta_delta_normalize(env, ctx, decl.properties.sort.get());
    return changed;
}

bool beta_delta_normalize(env_t const& env, func_def_t& def)
{
    ctx_t ctx;
    bool changed = false;
    for (func_arg_t& arg: def.value.args)
    {
        changed |= beta_delta_normalize(env, ctx, arg.type);
        auto const ok = ctx.try_emplace(arg.var, std::nullopt, ctx_t::var_decl_t{arg.qty, arg.type});
        assert(ok.has_value());
    }
    changed |= beta_delta_normalize(env, ctx, def.value.ret_type.get());
    changed |= beta_delta_normalize(env, ctx, def.value.body);
    changed |= beta_delta_normalize(env, ctx, def.properties.sort.get());
    return changed;
}

bool beta_delta_normalize(env_t const& env, ctx_t const& ctx, sort_t& sort)
{
    return match(
        sort,
        [&] (expr_t& type) { return beta_delta_normalize(env, ctx, type); },
        [] (kind_t const&) { return false; });
}

bool beta_delta_normalize(env_t const& env, ctx_t const& ctx, body_t& body)
{
    bool changed = beta_normalize(body);
    while (delta_unfold(env, ctx, body))
    {
        changed = true;
        beta_normalize(body);
    }
    return changed;
}

} // namespace impl

bool beta_delta_normalize(module_t& m)
{
    env_t env;
    bool changed = false;
    for (auto& x: m.entries)
        // normalize first and then store in env, so future look-ups will find the normalized term
        changed |= match(
            x,
            [&] (type_def_t const& def)
            {
                // nothing to normalize but still need to add the definition in the environment
                auto const& name = match(def.value, [&] (auto const& x) { return x.name; });
                auto const ok = env.try_emplace(expr_t::global_t{std::nullopt, name}, def);
                assert(ok.has_value());
                return false;
            },
            [&] (axiom_t& axiom)
            {
                bool const result = impl::beta_delta_normalize(env, axiom);
                auto const ok = env.try_emplace(expr_t::global_t{std::nullopt, axiom.name}, axiom);
                assert(ok.has_value());
                return result;
            },
            [] (extern_decl_t&)
            {
                // extern declarations cannot contain dependencies or other type-expressions,
                // so there is nothing to normalize here
                return false;
            },
            [&] (func_decl_t& decl)
            {
                bool const result = impl::beta_delta_normalize(env, decl);
                auto const ok = env.try_emplace(expr_t::global_t{std::nullopt, decl.name}, decl);
                assert(ok.has_value());
                return result;
            },
            [&] (func_def_t& def)
            {
                bool const result = impl::beta_delta_normalize(env, def);
                auto const ok = env.try_emplace(expr_t::global_t{std::nullopt, def.name}, def);
                assert(ok.has_value());
                return result;
            });
    return changed;
}

bool beta_delta_normalize(env_t const& env, ctx_t const& ctx, expr_t& expr)
{
    bool changed = beta_normalize(expr);
    while (delta_unfold(env, ctx, expr))
    {
        changed = true;
        beta_normalize(expr);
    }
    return changed;
}

} // namespace dep0::typecheck
