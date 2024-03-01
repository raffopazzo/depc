#include "private/delta_unfold.hpp"

namespace dep0::typecheck {

bool delta_unfold(environment_t const& env, context_t const& ctx, expr_t& x)
{
    if (auto const global = std::get_if<expr_t::global_t>(&x.value))
        if (auto const func_def = std::get_if<func_def_t>(env[*global]))
        {
            x.value = func_def->value;
            return true;
        }
    return false;
}

} // namespace dep0::typecheck
