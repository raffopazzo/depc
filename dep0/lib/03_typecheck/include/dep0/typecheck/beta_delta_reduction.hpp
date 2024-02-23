/**
 * @file beta_delta_reduction.hpp
 * @brief A family of functions that perform a combined beta-delta normalization.
 *
 * The normalization strategy is to first apply a full beta-normalization,
 * which should help in reducing the size of the AST.
 * If no more beta-normalizations are possible, try to perform a single one-step delta-reduction,
 * (arguably it should be called a delta-expansion) which might unblock further beta-normalizations.
 * If delta-reduction was unsuccessful, then we have reached a beta-delta normal-form.
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
