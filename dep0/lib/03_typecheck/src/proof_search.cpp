#include "private/proof_search.hpp"

#include "private/tactics/search_app.hpp"
#include "private/tactics/search_true_t.hpp"
#include "private/tactics/search_var.hpp"

namespace dep0::typecheck {

std::optional<expr_t>
proof_search(
    environment_t const& env,
    context_t const& ctx,
    expr_t const& type,
    usage_t& usage,
    ast::qty_t const usage_multiplier)
{
    // Try a bunch of tactics until the first one succeeds.
    std::optional<expr_t> result;
    auto const try_one = [&] (auto&& f) { if (not result) result = f(env, ctx, type, usage, usage_multiplier); };
    auto const try_all = [&] (auto&&... tactics) { (try_one(tactics), ...); };
    try_all(
        search_true_t,
        search_var,
        search_app
    );
    return result;
}

} // namespace dep0::typecheck

