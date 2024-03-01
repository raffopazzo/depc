/**
 * @file beta_delta_reduction.hpp
 * @brief A family of functions that perform a combined beta-delta normalization.
 *
 * Note that delta-unfolding is only performed inside a direct application,
 * eg `f(x)` for some global function `f`, but not everywhere, eg `return f`.
 * The reason for this is that we are interested in delta-unfolding only as
 * a way to extend beta normalization as far as possible.
 */

#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/environment.hpp"

namespace dep0::typecheck {

bool beta_delta_normalize(module_t&);
bool beta_delta_normalize(environment_t const&, func_decl_t&);
bool beta_delta_normalize(environment_t const&, func_def_t&);
bool beta_delta_normalize(environment_t const&, context_t const&, body_t&);
bool beta_delta_normalize(environment_t const&, context_t const&, expr_t&);

} // namespace dep0::typecheck
