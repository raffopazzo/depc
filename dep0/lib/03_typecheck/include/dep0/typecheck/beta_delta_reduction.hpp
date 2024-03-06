#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/environment.hpp"

namespace dep0::typecheck {

/**
 * Perform a combined beta-delta normalization inside the given module.
 *
 * Note that delta-unfolding is only performed inside a direct application,
 * eg `f(x)` for some global function `f`, but not everywhere, eg `return f`.
 * The reason for this is that we are interested in delta-unfolding only as
 * a way to extend beta normalization as far as possible.
 */
bool beta_delta_normalize(module_t&);

/**
 * Perform a combined beta-delta normalization inside the given expression.
 *
 * Note that delta-unfolding is only performed inside a direct application,
 * eg `f(x)` for some global function `f`, but not everywhere, eg `return f`.
 * The reason for this is that we are interested in delta-unfolding only as
 * a way to extend beta normalization as far as possible.
 */
bool beta_delta_normalize(environment_t const&, context_t const&, expr_t&);

} // namespace dep0::typecheck
