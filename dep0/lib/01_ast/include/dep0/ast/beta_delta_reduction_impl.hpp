#pragma once

#include "dep0/ast/beta_delta_reduction.hpp"
#include "dep0/ast/beta_reduction.hpp"

#include "dep0/match.hpp"

#include <cassert>

namespace dep0::ast {

template <Properties P>
bool beta_delta_normalize(delta_reduction::context_t<P> const& ctx, module_t<P>& m)
{
    auto ctx2 = ctx.extend();
    bool changed = beta_normalize(m);
    for (auto& def: m.func_defs)
    {
        changed |= beta_delta_normalize(ctx2, def);
        // store the result in ctx2, so future look-ups of this definition will find the normalized version
        bool const inserted = ctx2.try_emplace(typename expr_t<P>::var_t{indexed_var_t{def.name}}, def.value).second;
        assert(inserted);
    }
    return changed;
}

template <Properties P>
bool beta_delta_normalize(delta_reduction::context_t<P> const& ctx, func_def_t<P>& def)
{
    auto ctx2 = ctx.extend();
    for (auto const& arg: def.value.args)
        match(
            arg.value,
            [] (typename func_arg_t<P>::type_arg_t const&) { }, // TODO add test because this is wrong
            [&] (typename func_arg_t<P>::term_arg_t const& term_arg)
            {
                if (term_arg.var)
                {
                    bool const inserted = ctx2.try_emplace(*term_arg.var, delta_reduction::something_else_t{}).second;
                    assert(inserted);
                }
            });
    return beta_delta_normalize(ctx2, def.value.body);
}

template <Properties P>
bool beta_delta_normalize(delta_reduction::context_t<P> const& ctx, body_t<P>& body)
{
    bool changed = false;
    for (auto& s: body.stmts)
        changed |= beta_delta_normalize(ctx, s);
    return changed;
}

template <Properties P>
bool beta_delta_normalize(delta_reduction::context_t<P> const& ctx, stmt_t<P>& x)
{
    bool changed = beta_normalize(x);
    while (delta_reduce(ctx, x))
    {
        changed = true;
        beta_normalize(x);
    }
    return changed;
}

template <Properties P>
bool beta_delta_normalize(delta_reduction::context_t<P> const& ctx, expr_t<P>& x)
{
    bool changed = beta_normalize(x);
    while (delta_reduce(ctx, x))
    {
        changed = true;
        beta_normalize(x);
    }
    return changed;
}

template <Properties P>
bool beta_delta_normalize(delta_reduction::context_t<P> const& ctx, type_t<P>& x)
{
    bool changed = beta_normalize(x);
    while (delta_reduce(ctx, x))
    {
        changed = true;
        beta_normalize(x);
    }
    return changed;
}

} // namespace dep0::ast

