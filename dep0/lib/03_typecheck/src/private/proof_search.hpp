#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/environment.hpp"

#include <optional>

namespace dep0::typecheck {

/**
 * Search for a value of the given type in the given environment and context.
 * If none can be found, returns an empty optional.
 */
std::optional<expr_t> proof_search(environment_t const&, context_t const&, expr_t const& type);

} // namespace dep0::typecheck
