/**
 * @file delta_reduction.hpp
 * @brief A family of functions that perform a "simplified" one-step delta-reduction, not a complete normalization.
 *
 * By "simplified" we mean that one-step delta-reduction is not applied everywhere that is theoretically possible,
 * but only to function applications whose function expression is a variable name, eg `f(x, y, z)`.
 * In contrast, a "complete" one-step delta-reduction would normally expand `f` anywhere,
 * even if it appears on its own, for example inside `return f`.
 * The reason behind this decision is that we are interested in applying delta-reduction
 * only as a way to extend beta-normalization as far as possible.
 * Therefore there is no real value in expanding `f` when it appears on its own,
 * because this expansion will never be followed by a beta-reduction.
 * In fact, it may even have negative value, because during LLVM codegen we might end up
 * generating an anonymous private function when we could just refer to `f` by name.
 */

#pragma once

#include "dep0/typecheck/ast.hpp"
#include "dep0/typecheck/context.hpp"
#include "dep0/typecheck/environment.hpp"

namespace dep0::typecheck {

/**
 * Performs a "simplified" one-step delta-reduction inside a function definition, @see delta_reduction.hpp.
 *
 * @return True if one step of delta-reduction was performed, false otherwise.
 */
bool delta_reduce(environment_t const&, func_def_t&);

/**
 * Performs a "simplified" one-step delta-reduction inside a body, @see delta_reduction.hpp.
 *
 * @return True if one step of delta-reduction was performed, false otherwise.
 */
bool delta_reduce(environment_t const&, context_t const&, body_t&);

/**
 * Performs a "simplified" one-step delta-reduction inside a statement, @see delta_reduction.hpp.
 *
 * @return True if one step of delta-reduction was performed, false otherwise.
 */
bool delta_reduce(environment_t const&, context_t const&, stmt_t&);

/**
 * Performs a "simplified" one-step delta-reduction inside an expression, @see delta_reduction.hpp.
 *
 * @return True if one step of delta-reduction was performed, false otherwise.
 */
bool delta_reduce(environment_t const&, context_t const&, expr_t&);

} // namespace dep0::typecheck
