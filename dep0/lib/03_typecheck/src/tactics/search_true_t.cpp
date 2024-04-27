#include "private/tactics/search_true_t.hpp"

#include "private/derivation_rules.hpp"

#include "dep0/typecheck/beta_delta_reduction.hpp"

namespace dep0::typecheck {

std::optional<expr_t>
search_true_t(
    environment_t const& env,
    context_t const& ctx,
    expr_t const& type,
    usage_t&,
    ast::qty_t)
{
    std::optional<expr_t> result;
    if (auto const app = std::get_if<expr_t::app_t>(&type.value);
        app and std::holds_alternative<expr_t::true_t>(app->func.get().value))
    {
        auto expr = app->args[0];
        beta_delta_normalize(env, ctx, expr);
        auto const c = std::get_if<expr_t::boolean_constant_t>(&expr.value);
        if (c and c->value)
            result.emplace(make_legal_expr(type, expr_t::init_list_t{}));
    }
    return result;
}

} // namespace dep0::typecheck
