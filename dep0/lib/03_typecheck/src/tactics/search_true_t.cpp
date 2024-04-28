#include "private/tactics/search_true_t.hpp"

#include "private/beta_delta_equivalence.hpp"
#include "private/derivation_rules.hpp"

#include "dep0/typecheck/beta_delta_reduction.hpp"

namespace dep0::typecheck {

/**
 * @return If the given expression is some `true_t(cond)` return a pointer to `cond`; null otherwise.
 */
static expr_t const* try_extract_condition(expr_t const& x)
{
    auto const app = std::get_if<expr_t::app_t>(&x.value);
    return app and std::holds_alternative<expr_t::true_t>(app->func.get().value) ? &app->args[0] : nullptr;
}

/**
 * @return True if the given expression is the boolean constant `true`; false otherwise.
 */
static bool is_true(expr_t const& x)
{
    auto const c = std::get_if<expr_t::boolean_constant_t>(&x.value);
    return c and c->value;
}

std::optional<expr_t>
search_true_t(
    environment_t const& env,
    context_t const& ctx,
    expr_t const& type,
    usage_t&,
    ast::qty_t)
{
    if (auto const cond = try_extract_condition(type))
    {
        // The condition might aready be true.
        if (is_true(*cond))
            return make_legal_expr(type, expr_t::init_list_t{});

        // Or perhaps we can reduce it to true.
        if (auto copy = *cond; beta_delta_normalize(env, ctx, copy) and is_true(copy))
            return make_legal_expr(type, expr_t::init_list_t{});

        // Perhaps we have already proved that the condition was true?
        // If so we should have a proof in the context which allows us to return `{}`.
        // Note that this is different from returning the variable from the context because
        // we don't need to care about quantities.
        for (auto const& v: ctx.vars())
        {
            auto const& val = ctx[v]->value;
            if (auto const cond2 = try_extract_condition(val.type))
                if (is_beta_delta_equivalent(env, ctx, *cond, *cond2))
                    return make_legal_expr(type, expr_t::init_list_t{});
        }
    }
    return std::nullopt;
}

} // namespace dep0::typecheck
