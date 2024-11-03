#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/environment.hpp"

#include "private/proof_search.hpp"
#include "private/usage.hpp"

#include <optional>

namespace dep0::typecheck {

/**
 * Search for a value of type `true_t(expr)` for some boolean `expr`.
 * If `expr` is provably true, the returned value will be the empty initializer list `{}`,
 * because `true_t(true)` is a unit-like type.
 *
 * @remarks This tactic will either succeed or fail immediately and is therefore used by `quick_search`.
 */
void search_true_t(search_task_t&);

} // namespace dep0::typecheck

