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
        auto const& val = task.ctx[v]->value;
        if (is_beta_delta_equivalent(task.env, task.ctx, val.type, sort))
            if (auto const q = ast::qty_t::one * task.usage_multiplier; (usage[v] + q) <= val.qty)
            {
                usage.add(v, q);
                task.set_result(make_legal_expr(val.type, v));
                return;
            }
    }
}

} // namespace dep0::typecheck
