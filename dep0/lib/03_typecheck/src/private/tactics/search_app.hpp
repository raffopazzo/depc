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
 *
 * @remarks This tactic may not complete immediately and is therefore **not** used by `quick_search`.
 */
void search_app(search_task_t&);

} // namespace dep0::typecheck
