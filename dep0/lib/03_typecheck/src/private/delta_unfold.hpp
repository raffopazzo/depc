#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/environment.hpp"

namespace dep0::typecheck {

/**
 * Unfold one function definition,
 * from the given environment and context,
 * inside the given expression.
 *
 * @return True if a definition was unfolded.
 */
bool delta_unfold(environment_t const&, context_t const&, expr_t&);

} // namespace dep0::typecheck
