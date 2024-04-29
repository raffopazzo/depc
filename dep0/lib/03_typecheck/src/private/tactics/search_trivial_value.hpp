#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/environment.hpp"

#include "private/proof_search.hpp"
#include "private/usage.hpp"

#include <optional>

namespace dep0::typecheck {

/**
 * If the given type is inhabited by a single value, return that value.
 * For example, user-defined integeral whose only value is 0 or `array_t(t, 0)` whose only value is `{}`.
 */
std::optional<expr_t>
search_trivial_value(
    environment_t const&,
    context_t const&,
    expr_t const&,
    search_state_t&,
    usage_t&,
    ast::qty_t usage_multiplier);

} // namespace dep0::typecheck
