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
    for (ctx_t::decl_t const& decl: task.ctx.decls())
        if (is_beta_delta_equivalent(decl.type, *task.target))
            if (usage.try_add(decl, task.usage_multiplier))
                return task.set_result(make_legal_expr(task.env, task.ctx, decl.type, decl.var));
}

} // namespace dep0::typecheck
