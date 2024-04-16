#include "private/tactics/search_var.hpp"

#include "private/beta_delta_equivalence.hpp"
#include "private/derivation_rules.hpp"

namespace dep0::typecheck {

std::optional<expr_t> search_var(environment_t const& env, context_t const& ctx, expr_t const& type)
{
    // TODO search inside environment
    sort_t const sort = type;
    for (auto const& v: ctx.vars())
    {
        auto const& val = ctx[v]->value;
        if (is_beta_delta_equivalent(env, ctx, val.type, sort))
            return make_legal_expr(val.type, v);
    }
    return std::nullopt;
}

} // namespace dep0::typecheck

