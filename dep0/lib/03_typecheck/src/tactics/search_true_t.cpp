/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/tactics/search_true_t.hpp"

#include "private/beta_delta_equivalence.hpp"
#include "private/derivation_rules.hpp"

#include "dep0/typecheck/beta_delta_reduction.hpp"

namespace dep0::typecheck {

/** If the given expression is some `true_t(cond)` return a pointer to `cond`; null otherwise. */
static expr_t const* try_extract_condition(expr_t const& x)
{
    auto const app = std::get_if<expr_t::app_t>(&x.value);
    return app and std::holds_alternative<expr_t::true_t>(app->func.get().value) ? &app->args[0] : nullptr;
}

/** Return true if the given expression is the boolean constant `true`; false otherwise. */
static bool is_true(expr_t const& x)
{
    auto const c = std::get_if<expr_t::boolean_constant_t>(&x.value);
    return c and c->value;
}

void search_true_t(search_task_t& task)
{
    auto const& target = *task.target;
    if (auto const cond = try_extract_condition(target))
    {
        // The condition might already be true.
        if (is_true(*cond))
            return task.set_result(make_legal_expr(task.env, task.ctx, target, expr_t::init_list_t{}));

        // Or perhaps we can reduce it to true.
        if (auto copy = *cond; beta_delta_normalize(copy) and is_true(copy))
            return task.set_result(make_legal_expr(task.env, task.ctx, target, expr_t::init_list_t{}));

        // Perhaps we have already proved that the condition was true?
        // If so we should have a proof in the context which allows us to return `{}`.
        // Note that this is different from returning the variable from the context because
        // we don't need to care about quantities.
        for (ctx_t::decl_t const& decl: task.ctx.decls())
            if (auto const cond2 = try_extract_condition(decl.type))
                if (is_beta_delta_equivalent(*cond, *cond2))
                    return task.set_result(make_legal_expr(task.env, task.ctx, target, expr_t::init_list_t{}));
    }
}

} // namespace dep0::typecheck
