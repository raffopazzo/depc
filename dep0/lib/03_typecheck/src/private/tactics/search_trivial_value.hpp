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
 * For example, user-defined integral whose only value is 0 or `array_t(t, 0)` whose only value is `{}`.
 */
void search_trivial_value(search_task_t&);

} // namespace dep0::typecheck
