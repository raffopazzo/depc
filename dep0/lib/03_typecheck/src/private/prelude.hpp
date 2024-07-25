#pragma once

#include "dep0/typecheck/ast.hpp"

#include "dep0/error.hpp"

namespace dep0::typecheck {

/**
 * Build and return the prelude module.
 * The prelude module contains fundamental axioms
 * of primitive types and basic logic inference rules
 * which are required to reason about user programmes;
 * for example the axioms of double negation for booleans.
 */
dep0::expected<module_t> build_prelude_module();

} // namespace dep0::typecheck
