#include "dep0/transform/beta_delta_reduction.hpp"

#include "dep0/transform/beta_reduction.hpp"

#include "dep0/match.hpp"

#include <cassert>

namespace dep0::transform {

expected<std::true_type> beta_delta_normalization_t::operator()(typecheck::module_t& m) const
{
    beta_delta_normalize(ctx, m);
    return std::true_type{};
}

template <typename T>
bool normalize(delta_reduction::context_t const& ctx, T& x)
{
    bool changed = beta_normalize(x);
    while (delta_reduce(ctx, x))
    {
        changed = true;
        beta_normalize(x);
    }
    return changed;
}

bool beta_delta_normalize(delta_reduction::context_t const& ctx, typecheck::module_t& m)
{
    auto ctx2 = ctx.extend();
    bool changed = beta_normalize(m);
    for (auto& def: m.func_defs)
    {
        changed |= beta_delta_normalize(ctx2, def);
        // store the result in ctx2, so future look-ups of this definition will find the normalized version
        bool const inserted = ctx2.try_emplace(typecheck::expr_t::var_t{ast::indexed_var_t{def.name}}, def.value).second;
        assert(inserted);
    }
    return changed;
}

bool beta_delta_normalize(delta_reduction::context_t const& ctx, typecheck::func_def_t& def)
{
    auto ctx2 = ctx.extend();
    for (auto const& arg: def.value.args)
        match(
            arg.value,
            [] (typecheck::func_arg_t::type_arg_t const&) { }, // TODO add test because this is wrong
            [&] (typecheck::func_arg_t::term_arg_t const& term_arg)
            {
                if (term_arg.var)
                {
                    bool const inserted = ctx2.try_emplace(*term_arg.var, delta_reduction::something_else_t{}).second;
                    assert(inserted);
                }
            });
    return beta_delta_normalize(ctx2, def.value.body);
}

bool beta_delta_normalize(delta_reduction::context_t const& ctx, typecheck::body_t& body)
{
    bool changed = false;
    for (auto& s: body.stmts)
        changed |= beta_delta_normalize(ctx, s);
    return changed;
}

bool beta_delta_normalize(delta_reduction::context_t const& ctx, typecheck::stmt_t& x) { return normalize(ctx, x); }
bool beta_delta_normalize(delta_reduction::context_t const& ctx, typecheck::expr_t& x) { return normalize(ctx, x); }
bool beta_delta_normalize(delta_reduction::context_t const& ctx, typecheck::type_t& x) { return normalize(ctx, x); }

} // namespace dep0::transform

