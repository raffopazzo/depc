#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/environment.hpp"

#include "private/proof_search.hpp"
#include "private/usage.hpp"

#include <optional>

namespace dep0::typecheck {

/**
 * Search for a value of the given type by applying axioms and theorems (aka functions) from the given environment.
 */
std::optional<expr_t>
search_app(
    env_t const&,
    ctx_t const&,
    expr_t const& type,
    search_state_t&,
    usage_t&,
    ast::qty_t usage_multiplier);

} // namespace dep0::typecheck
