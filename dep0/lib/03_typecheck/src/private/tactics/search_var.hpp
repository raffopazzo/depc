#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/environment.hpp"

#include "private/proof_search.hpp"
#include "private/usage.hpp"

#include <optional>

namespace dep0::typecheck {

/**
 * Search for a value of the given type inside the given environemnt or context.
 * If the environemnt contains a declaration or definition of the desired type,
 * the returned value will be a `global_t` referring to it.
 * If the context contains a declaration of the desired type,
 * the returned value will be a `var_t` referring to it.
 * Otherwise search fails and `std::nullopt` is returned.
 */
void search_var(search_task_t&);

} // namespace dep0::typecheck
