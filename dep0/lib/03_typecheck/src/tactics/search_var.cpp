/*
 * Copyright Raffaele Rossi 2023 - 2024.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "private/tactics/search_var.hpp"

#include "private/beta_delta_equivalence.hpp"
#include "private/derivation_rules.hpp"

namespace dep0::typecheck {

void search_var(search_task_t& task)
{
    // TODO search inside environment
    auto& usage = *task.usage;
    sort_t const sort = *task.target;
    for (auto const& v: task.ctx.vars())
    {
        auto lookup = context_lookup(task.ctx, v);
        assert(lookup.has_value());
        if (is_beta_delta_equivalent(task.env, task.ctx, lookup->decl.type, sort))
            if (usage.try_add(*lookup, task.usage_multiplier))
                return task.set_result(make_legal_expr(lookup->decl.type, v));
    }
}

} // namespace dep0::typecheck
