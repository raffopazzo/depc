#include "private/proof_search.hpp"

#include "private/tactics/search_true_t.hpp"

namespace dep0::typecheck {

std::optional<expr_t> proof_search(environment_t const& env, context_t const& ctx, expr_t const& type)
{
    // Try a bunch of tactics until the first one succeeds.
    std::optional<expr_t> result;
    auto const try_one = [&] (auto&& tactic) { if (not result) result = tactic(env, ctx, type); };
    auto const try_all = [&] (auto&&... tactics) { (try_one(tactics), ...); };
    try_all(
        search_true_t
    );
    return result;
}

} // namespace dep0::typecheck

