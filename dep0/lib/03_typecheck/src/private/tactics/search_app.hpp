#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/environment.hpp"

#include <optional>

namespace dep0::typecheck {

/**
 * Search for a value of the given type by applying axioms and theorems (aka functions) from the given environment.
 */
std::optional<expr_t> search_app(environment_t const&, context_t const&, expr_t const& type);

} // namespace dep0::typecheck